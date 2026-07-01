#ifndef APPLICATION_H
#define APPLICATION_H

#include "data/declarations.h"

void InitializeApplication();

void SetApplicationName(const char* name);

void SetApplicationGoodbye(const char* goodbye);

void SetApplicationSize(const size_t width, const size_t height);

void RunApplication();

void DestroyApplication();

void AddScene(Scene* scene);

BOOL HasScene(const char* name);

void SetScene(const char* name);

Scene* GetActiveScene();

BOOL Playing();

void Pause();

void Resume();

BOOL IsFast();

void ToggleFastForward();

void Step(size_t steps);

ARRLIST_StaticString* SceneNames();

#endif
