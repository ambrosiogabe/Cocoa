#pragma once

#include "jade/renderer/Texture.h"
#include "jade/util/Log.h"

#include <entt/entt.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <vector>

namespace Jade {
    struct Sprite {
        const char* m_PictureFile = nullptr;
        int m_Width = 0;
        int m_Height = 0;
        Texture* m_Texture = nullptr;
        glm::vec2 m_TexCoords[4] = {
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(0.0f, 1.0f)
        };
    };

    class Spritesheet {
    public:
        Texture* m_Texture;
        std::vector<Sprite> m_Sprites;

        Spritesheet(Texture* texture, int spriteWidth, int spriteHeight, int numSprites, int spacing) {
            // NOTE: If you don't reserve the space before hand, when the vector grows it may
            // change the pointers it holds
            m_Sprites.reserve(numSprites);

            m_Texture = texture;
            int currentX = 0;
            int currentY = m_Texture->GetHeight() - spriteHeight;
            for (int i=0; i < numSprites; i++) {
                float topY = (currentY + spriteHeight) / (float)texture->GetHeight();
                float rightX = (currentX + spriteWidth) / (float)texture->GetWidth();
                float leftX = currentX / (float)texture->GetWidth();
                float bottomY = currentY / (float)texture->GetHeight();

                Sprite sprite = {
                    texture->GetFilepath(),
                    spriteWidth,
                    spriteHeight,
                    texture, {
                        glm::vec2(rightX, topY),
                        glm::vec2(rightX, bottomY),
                        glm::vec2(leftX, bottomY),
                        glm::vec2(leftX, topY)
                    }
                };
                m_Sprites.push_back(sprite);

                currentX += spriteWidth + spacing;
                if (currentX >= texture->GetWidth()) {
                    currentX = 0;
                    currentY -= spriteHeight + spacing;
                }
            }
        }

        Sprite* GetSprite(int index) {
            return &m_Sprites[index];
        }
    };

    static Sprite* defaultSprite = new Sprite();
    struct SpriteRenderer {
        glm::vec4 m_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        Sprite* m_Sprite = defaultSprite;
    };

    struct Transform {
        Transform() {
            Init(glm::vec3(0), glm::vec3(1), glm::vec3(0));
        }

        Transform(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation, const char* name="New GameObject", 
                    entt::entity parent=entt::null, entt::entity previous=entt::null) {
            Init(position, scale, eulerRotation, name, parent, previous);
        }

        void UpdateMatrices() {
            m_ModelMatrix = glm::translate(glm::mat4(1.0f), m_Position);
            m_ModelMatrix = m_ModelMatrix * glm::toMat4(m_Orientation);
            m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
        }

        entt::entity m_Parent;
        entt::entity m_Previous;
        entt::entity m_Next;
        entt::entity m_First;

        glm::vec3 m_Position;
        glm::vec3 m_Scale;
        glm::vec3 m_EulerRotation;
        glm::quat m_Orientation;

        glm::vec3 m_Forward;
        glm::vec3 m_Up;
        glm::vec3 m_Right;

        glm::mat4 m_ModelMatrix;
        glm::mat4 m_InverseModelMatrix;
        const char* m_Name;

    private:
        void Init(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation, const char* name="New GameObject", 
            entt::entity parent=entt::null, entt::entity previous=entt::null) {
            m_Position = position;
            m_Scale = scale;
            m_EulerRotation = eulerRotation;
            m_Orientation = glm::toQuat(glm::orientate3(m_EulerRotation));

            m_Forward = glm::vec3(0, 0, 1) * m_Orientation;
            m_Up = glm::vec3(0, 1, 0) * m_Orientation;
            m_Right = glm::vec3(1, 0, 0) * m_Orientation;
            m_Name = name;
            m_Parent = parent;
            m_Previous = previous;
            m_First = entt::null;
            m_Next = entt::null;

            this->UpdateMatrices();
        }
    };

}