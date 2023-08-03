#pragma once

#include "FileSystem.h"
#include "Macro.h"

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

namespace VRaytracer
{
    struct Vector3Info
    {
        double X;
        double Y;
        double Z;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(CEREAL_NVP(X), CEREAL_NVP(Y), CEREAL_NVP(Z));
        }
    };

    using Point3Info = Vector3Info;
    using ColorInfo = Vector3Info;

    struct CameraConfiguration
    {
        Point3Info  LookFrom;
        Point3Info  LookAt;
        Vector3Info ViewUp;
        double      DistanceToFocus;
        double      Aperture;
        double      FOV;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(CEREAL_NVP(LookFrom),
                    CEREAL_NVP(LookAt),
                    CEREAL_NVP(ViewUp),
                    CEREAL_NVP(DistanceToFocus),
                    CEREAL_NVP(Aperture),
                    CEREAL_NVP(FOV));
        }
    };

    struct SceneConfiguration
    {
        CameraConfiguration CameraConfig;
        ColorInfo    BackgroundColor;

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(CEREAL_NVP(CameraConfig), CEREAL_NVP(BackgroundColor));
        }

        template<class Archive>
        void load(Archive& archive)
        {
            archive(CEREAL_NVP(CameraConfig), CEREAL_NVP(BackgroundColor));
        }
    };

    class ConfigLoader
    {
    public:
        static Ref<SceneConfiguration> LoadBuiltinScene(std::string sceneName)
        {
            std::filesystem::path scenePath = FileSystem::GetExecutableRelativeDirectory("Resources/Scenes/" + sceneName + s_ConfigSuffix);
            if (!std::filesystem::exists(scenePath))
            {
                VRT_ERROR("Scene configuration not found at {0}!", scenePath);
                return nullptr;
            }

            SceneConfiguration sceneConfig;

            std::ifstream            is(scenePath);
            cereal::JSONInputArchive archive(is);
            archive(cereal::make_nvp("Scene", sceneConfig));

            return CreateRef<SceneConfiguration>(sceneConfig);
        }

    private:
        static const std::string s_ConfigSuffix;
    };

    const std::string ConfigLoader::s_ConfigSuffix = ".json";
}; // namespace VRaytracer