#include "stdafx.h"
#include "Game.h"
#include "Light.h"
#include "Camera.h"


Light::Light()
{
    initTransformation();
    shadowMappingEnabled = false;
    shadowMapWidth = 1;
    shadowMapHeight = 1;
    cutOffDistance = 10000;
    angle = deg2rad(90);
    lightCamera = new Camera();
    delete lightCamera->transformation;
}

Light::~Light()
{
    destroyFbo();
}

void Light::resizeShadowMap(int width, int height)
{
    if (shadowMapWidth == width && shadowMapHeight == height) return;

    shadowMapWidth = width;
    shadowMapHeight = height; 

    recreateFbo();
}

void Light::switchShadowMapping(bool value)
{
    if (shadowMappingEnabled == value) return;
    if (value) {
        shadowMappingEnabled = true;
        recreateFbo();
    }
    else {
        shadowMappingEnabled = false;
        destroyFbo();
    }
}

void Light::bindShadowMap(int index)
{
    if (shadowMappingEnabled) {
        depthMap->use(index);
    }
}

void Light::refreshShadowMap()
{
    if (shadowMappingEnabled) {
        mapper->use(true);
        ShaderProgram *shader = Game::instance->shaders->depthOnlyShader;
        shader->use();
        shader->setUniform("CutOffDistance", cutOffDistance);
        lightCamera->transformation = transformation;
        lightCamera->createProjectionPerspective(angle, (float)shadowMapWidth / (float)shadowMapHeight, 0.1f, cutOffDistance);
        Game::instance->world->draw(shader, lightCamera);
    }
}

void Light::recreateFbo()
{
    destroyFbo();
    Texture* helperDepthBuffer = new Texture(shadowMapWidth, shadowMapHeight,
        GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT);
    depthMap = new Texture(shadowMapWidth, shadowMapHeight,
        GL_R32F, GL_RED, GL_FLOAT);
    mapper = new Framebuffer();
    mapper->attachTexture(helperDepthBuffer, GL_DEPTH_ATTACHMENT);
    mapper->attachTexture(depthMap, GL_COLOR_ATTACHMENT0);
}

void Light::destroyFbo()
{
    if (mapper != nullptr) {
        delete mapper;
        delete depthMap;
    }
}