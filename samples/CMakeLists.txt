

set(tutorials
01-create-window
02-hello-dot-fix-pipeline
03-hello-triangle-fix-pipeline
04-hello-triangle
05-uniform-variables
06-translation-transformation
)

foreach(tutorial ${tutorials})
    add_subdirectory(${tutorial})
endforeach()



