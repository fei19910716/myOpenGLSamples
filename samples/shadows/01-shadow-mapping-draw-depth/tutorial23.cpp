/*

        Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Tutorial 23 - Shadow Mapping - Part 1
*/

#include <math.h>

#include "base/pipeline.h"
#include "base/math_3d.h"
#include "base/dev_backend.h"
#include "base/texture.h"
#include "base/app.h"

#include "base/lighting/lights_common.h"
#include "base/lighting/basic_lighting_technique.h"


#include "mesh.h"
#include "quad_mesh.h"
#include "shadow_map_technique.h"
#include "draw_texture_technique.h"
#include "shadow_map_fbo.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1200

class Tutorial23 : public ICallbacks, public OgldevApp
{
public:

    Tutorial23()
    {
        m_pShadowMapTech = NULL;
        m_pGameCamera = NULL;
        m_pMesh = NULL;
        m_pQuad = NULL;
        m_scale = 0.0f;

        m_spotLight.AmbientIntensity = 0.0f;
        m_spotLight.DiffuseIntensity = 0.9f;
        m_spotLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_spotLight.Attenuation.Linear = 0.01f;
        m_spotLight.Position  = Vector3f(-20.0, 20.0, 5.0f);
        m_spotLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);
        m_spotLight.Cutoff =  20.0f;

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 50.0f;
    }

    ~Tutorial23()
    {
        SAFE_DELETE(m_pShadowMapTech);
        SAFE_DELETE(m_pGameCamera);
        SAFE_DELETE(m_pMesh);
        SAFE_DELETE(m_pQuad);
    }

    bool Init()
    {
        /**
         * Create the shadow map FBO, which is used as the render target in the shadow map pass.
         * The FBO only has depth attachment. And will be used as texture in the later renderpass.
        */
        if (!m_shadowMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        }

        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

        m_pShadowMapTech = new ShadowMapTechnique();
        if (!m_pShadowMapTech->Init()) {
            printf("Error initializing the shadow map technique\n");
            return false;
        }

        m_pDrawTextureTech = new DrawTextureTechnique();
        if (!m_pDrawTextureTech->Init()) {
            printf("Error initializing the draw texture technique\n");
            return false;
        }

        m_pQuad = new QuadMesh();
        if (!m_pQuad->Init()) {
            return false;
        }

        m_pMesh = new Mesh();
        if (!m_pMesh->LoadMesh("models/phoenix_ugv.md2")) {
            return false;
        }

        // For debug
        m_pDebugTexture = new Texture(GL_TEXTURE_2D, "images/test.png");
        if (!m_pDebugTexture->Load()) {
            return false;
        }

        return true;
    }

    void Run()
    {
        OgldevBackendRun(this);
    }

    virtual void RenderSceneCB()
    {
        m_pGameCamera->OnRender();
        m_scale += 0.05f;

        ShadowMapPass();
        RenderPass();

        OgldevBackendSwapBuffers();
    }

    virtual void ShadowMapPass()
    {
        m_shadowMapFBO.BindForWriting();

        glClear(GL_DEPTH_BUFFER_BIT);

        Pipeline p;
        p.Scale(0.2f, 0.2f, 0.2f);
        p.Rotate(90.0f, 0.0, 0.0f);
        p.WorldPos(0.0f, 0.0f, 5.0f);
        p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        p.SetPerspectiveProj(m_persProjInfo);
        m_pShadowMapTech->Enable();
        m_pShadowMapTech->SetWVP(p.GetWVPTrans());
        m_pMesh->Render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    virtual void RenderPass()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT, GL_FILL); 
        
        m_shadowMapFBO.BindForReading(GL_TEXTURE0);
        // m_pDebug->Bind(GL_TEXTURE0);

        m_pDrawTextureTech->Enable();
        m_pDrawTextureTech->SetTextureUnit(GL_TEXTURE0);
        m_pQuad->Render();
    }


    void KeyboardCB(OGLDEV_KEY OgldevKey, OGLDEV_KEY_STATE State)
    {
        switch (OgldevKey) {
            case OGLDEV_KEY_ESCAPE:
            case OGLDEV_KEY_q:
                OgldevBackendLeaveMainLoop();
                break;
            default:
                m_pGameCamera->OnKeyboard(OgldevKey);
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {
        m_pGameCamera->OnMouse(x, y);
    }

 private:

    ShadowMapTechnique*   m_pShadowMapTech;
    DrawTextureTechnique* m_pDrawTextureTech;
    Camera*   m_pGameCamera;
    float     m_scale;
    SpotLight m_spotLight;
    Mesh*     m_pMesh;
    QuadMesh* m_pQuad;
    ShadowMapFBO  m_shadowMapFBO;
    PersProjInfo  m_persProjInfo;

    Texture*      m_pDebugTexture;
};


int main(int argc, char** argv)
{
//    Magick::InitializeMagick(*argv);
    OgldevBackendInit(argc, argv, true, false);

    if (!OgldevBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "Tutorial 23")) {
        return 1;
    }

    Tutorial23* pApp = new Tutorial23();

    if (!pApp->Init()) {
        return 1;
    }

    pApp->Run();

    delete pApp;

    return 0;
}
