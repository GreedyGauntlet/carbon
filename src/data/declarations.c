#include "declarations.h"
#include "data/definitions.h"
#include "ecs/entity.h"
#include <easyhash.h>

IMPL_ARRLIST(EntityID);
IMPL_ARRLIST(Script);
IMPL_ARRLIST(Texture2D);
IMPL_ARRLIST(Sound);
IMPL_ARRLIST(Music);
IMPL_ARRLIST(Animation);
IMPL_ARRLIST(Shader);
IMPL_ARRLIST(Entity);
IMPL_ARRLIST_NAMED(ScenePtr, Scene*);
IMPL_ARRLIST_NAMED(WorldPtr, World*);
IMPL_ARRLIST_NAMED(ComponentStoragePtr, ComponentStorage*);
IMPL_ARRLIST_NAMED(SystemPtr, System*);
IMPL_ARRLIST_NAMED(StaticString, const char*);
IMPL_ARRLIST_NAMED(DynamicString, char*);
IMPL_HASHMAP(int, BOOL, KeyMap, ez_hash_int);
IMPL_HASHMAP(EntityID, size_t, EntityMap, ez_hash_uint64_t);
IMPL_HASHMAP(size_t, ComponentStorage*, StorageMap, ez_hash_size_t);
IMPL_HASHMAP(EntityID, EntityID, Parents, ez_hash_uint64_t);
IMPL_HASHMAP(EntityID, ARRLIST_EntityID*, Children, ez_hash_uint64_t);
