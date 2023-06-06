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

    Tutorial 24 - Shadow Mapping - Part 2
*/

#include <math.h>

#include "base/pipeline.h"
#include "base/math_3d.h"
#include "base/dev_backend.h"
#include "base/texture.h"
#include "base/lighting/lights_common.h"
#include "base/app.h"
#include "lighting_technique.h"
#include "mesh.h"
#include "ground_mesh.h"
#include "shadow_map_technique.h"


#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1200


class Tutorial24 : public ICallbacks, public OgldevApp
{
public:

    Tutorial24()
    {
        m_pLightingEffect = NULL;
        m_pShadowMapEffect = NULL;
        m_pGameCamera = NULL;
        m_pMesh = NULL;
        m_pGround = NULL;
        m_scale = 0.0f;
        m_pGroundTex = NULL;

        m_spotLight.AmbientIntensity = 0.2f;
        m_spotLight.DiffuseIntensity = 0.8f;
        m_spotLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_spotLight.Attenuation.Linear = 0.01f;
        m_spotLight.Position  = Vector3f(-20.0, 20.0, 1.0f);
        m_spotLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);
        m_spotLight.Cutoff =  20.0f;

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 50.0f;
    }


    ~Tutorial24()
    {
        SAFE_DELETE(m_pLightingEffect);
        SAFE_DELETE(m_pShadowMapEffect);
        SAFE_DELETE(m_pGameCamera);
        SAFE_DELETE(m_pMesh);
        SAFE_DELETE(m_pGround);
        SAFE_DELETE(m_pGroundTex);
    }


    bool Init()
    {
        Vector3f Pos(3.0f, 8.0f, -10.0f);
        Vector3f Target(0.0f, -0.2f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        if (!m_shadowMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        }

        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

        m_pLightingEffect = new LightingTechnique();

        if (!m_pLightingEffect->Init()) {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pLightingEffect->Enable();
        m_pLightingEffect->SetSpotLights(1, &m_spotLight);
        m_pLightingEffect->SetTextureUnit(0);
        m_pLightingEffect->SetShadowMapTextureUnit(1);

        m_pShadowMapEffect = new ShadowMapTechnique();

        if (!m_pShadowMapEffect->Init()) {
            printf("Error initializing the shadow map technique\n");
            return false;
        }

        m_pGround = new GroundMesh();

        if (!m_pGround->Init()) {
            return false;
        }

        m_pMesh = new Mesh();

        if(!m_pMesh->LoadMesh("models/phoenix_ugv.md2")){
            return false;
        }

        m_pGroundTex = new Texture(GL_TEXTURE_2D, "images/test.png");

        if (!m_pGroundTex->Load()) {
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


    void ShadowMapPass()
    {
        m_shadowMapFBO.BindForWriting();

        glClear(GL_DEPTH_BUFFER_BIT);

        m_pShadowMapEffect->Enable();

        Pipeline p;
        p.Scale(0.1f, 0.1f, 0.1f);
        p.Rotate(0.0f, m_scale, 0.0f);
        p.WorldPos(0.0f, 0.0f, 3.0f);
        p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        p.SetPerspectiveProj(m_persProjInfo);
        m_pShadowMapEffect->SetWVP(p.GetWVPTrans());
        m_pMesh->Render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    void RenderPass()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pLightingEffect->Enable();

        m_pLightingEffect->SetEyeWorldPos(m_pGameCamera->GetPos());

        m_shadowMapFBO.BindForReading(GL_TEXTURE1);

        Pipeline p;
        p.SetPerspectiveProj(m_persProjInfo);

        p.Scale(4.0f, 4.0f, 4.0f);
        p.WorldPos(0.0f, 0.5f, 1.0f);
        p.Rotate(0.0f, 0.0f, 0.0f);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        m_pLightingEffect->SetWVP(p.GetWVPTrans());
        m_pLightingEffect->SetWorldMatrix(p.GetWorldTrans());
        p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        m_pLightingEffect->SetLightWVP(p.GetWVPTrans());
        m_pGroundTex->Bind(GL_TEXTURE0);
        m_pGround->Render();

        p.Scale(0.05f, 0.05f, 0.05f);
        p.Rotate(90.0f, m_scale, 0.0f);
        p.WorldPos(0.0f, 0.0f, 6.0f);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        m_pLightingEffect->SetWVP(p.GetWVPTrans());
        m_pLightingEffect->SetWorldMatrix(p.GetWorldTrans());
        p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        m_pLightingEffect->SetLightWVP(p.GetWVPTrans());
        m_pMesh->Render();
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

    LightingTechnique* m_pLightingEffect;
    ShadowMapTechnique* m_pShadowMapEffect;
    Camera* m_pGameCamera;
    float m_scale;
    SpotLight m_spotLight;
    Mesh* m_pMesh;
    GroundMesh* m_pGround;
    Texture* m_pGroundTex;
    ShadowMapFBO m_shadowMapFBO;
    PersProjInfo m_persProjInfo;
};


int main(int argc, char** argv)
{
    OgldevBackendInit(argc, argv, true, false);

    if (!OgldevBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "Tutorial 24")) {
        return 1;
    }

    Tutorial24* pApp = new Tutorial24();

    if (!pApp->Init()) {
        return 1;
    }

    pApp->Run();

    delete pApp;

    return 0;
}
