cmake_minimum_required(VERSION 3.27)
project(FinalProject)
include(CTest)
set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Resource)
include_directories(PlayerData)

file(COPY Resource/audios DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY allegro_monolith-5.2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(FinalProject
        Engine/Allegro5Exception.cpp
        Engine/Allegro5Exception.hpp
        Engine/AudioHelper.cpp
        Engine/AudioHelper.hpp
        Bullet/Bullet.cpp
        Bullet/Bullet.hpp
        Engine/Collider.cpp
        Engine/Collider.hpp
        UI/Animation/DirtyEffect.cpp
        UI/Animation/DirtyEffect.hpp
        UI/Animation/FireEffect.cpp
        UI/Animation/FireEffect.hpp
        Enemy/Enemy.cpp
        Enemy/Enemy.hpp
        UI/Animation/ExplosionEffect.cpp
        UI/Animation/ExplosionEffect.hpp
        UI/Animation/FireParticle.cpp
        UI/Animation/FireParticle.hpp
        UI/Animation/VisualEffect.cpp
        UI/Animation/VisualEffect.hpp
        Bullet/FireBullet.cpp
        Bullet/FireBullet.hpp
        Engine/GameEngine.cpp
        Engine/GameEngine.hpp
        Engine/Group.cpp
        Engine/Group.hpp
        Engine/IControl.cpp
        Engine/IControl.hpp
        UI/Component/Image.cpp
        UI/Component/Image.hpp
        UI/Component/ImageButton.cpp
        UI/Component/ImageButton.hpp
        Engine/IObject.cpp
        Engine/IObject.hpp
        Engine/IScene.cpp
        Engine/IScene.hpp
        UI/Component/Label.cpp
        UI/Component/Label.hpp
        Bullet/LaserBullet.cpp
        Bullet/LaserBullet.hpp
        Turret/LaserTurret.cpp
        Turret/LaserTurret.hpp
        Turret/Flamethrower.cpp
        Turret/Flamethrower.hpp
        Bullet/Flame.cpp
        Bullet/Flame.hpp
        Engine/LOG.cpp
        Engine/LOG.hpp
        Scene/LoseScene.cpp
        Scene/LoseScene.hpp
        Turret/MachineGunTurret.cpp
        Turret/MachineGunTurret.hpp
        main.cpp
        Bullet/MissileBullet.cpp
        Bullet/MissileBullet.hpp
        Turret/MissileTurret.cpp
        Turret/MissileTurret.hpp
        UI/Animation/Plane.cpp
        UI/Animation/Plane.hpp
        Enemy/PlaneEnemy.cpp
        Enemy/PlaneEnemy.hpp
        Scene/PlayScene.cpp
        Scene/PlayScene.hpp
        Engine/Point.cpp
        Engine/Point.hpp
        Engine/Resources.cpp
        Engine/Resources.hpp
        UI/Component/Slider.cpp
        UI/Component/Slider.hpp
        Enemy/SoldierEnemy.cpp
        Enemy/SoldierEnemy.hpp
        Engine/Sprite.cpp
        Engine/Sprite.hpp
        Engine/Unit.cpp
        Engine/Unit.hpp
        Scene/StageSelectScene.cpp
        Scene/StageSelectScene.hpp
        Enemy/TankEnemy.cpp
        Enemy/TankEnemy.hpp
        Enemy/TruckEnemy.cpp
        Enemy/TruckEnemy.hpp
        Turret/Turret.cpp
        Turret/Turret.hpp
        Turret/TurretButton.cpp
        Turret/TurretButton.hpp
        Scene/WinScene.cpp
        Scene/WinScene.hpp

        # DONE: [HACKATHON-1-SCENE] (2/4) Add the new scene source files here
        Scene/StartScene.cpp
        Scene/StartScene.h
        Scene/SettingsScene.cpp
        Scene/SettingsScene.hpp
        Scene/ScoreboardScene.cpp
        Scene/ScoreboardScene.hpp
        Turret/Wall.cpp
        Turret/Wall.hpp
        Enemy/SonicEnemy.cpp
        Enemy/SonicEnemy.h
        Enemy/EnemyButton.cpp
        Enemy/EnemyButton.h
        Scene/MapEditScene.cpp
        Scene/MapEditScene.hpp
        #        UI/Component/TextInput.cpp
#        UI/Component/TextInput.hpp
)

IF (WIN32)
    target_include_directories(FinalProject PUBLIC C:/allegro/include)
    target_link_directories(FinalProject PUBLIC C:/allegro/lib)
    target_compile_options(FinalProject PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(FinalProject liballegro_monolith)
ELSE()
    find_package(PkgConfig)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    target_include_directories(FinalProject PUBLIC lib)
    target_include_directories(FinalProject PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
    target_compile_options(FinalProject PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(FinalProject ${ALLEGRO5_LIBRARIES})
    target_link_directories(FinalProject PUBLIC ${ALLEGRO5_LIBRARY_DIRS})
ENDIF()