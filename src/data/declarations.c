#include "declarations.h"
#include "data/definitions.h"
#include <easyhash.h>

IMPL_ARRLIST(EntityID);
IMPL_ARRLIST(Panel);
IMPL_ARRLIST(Notification);
IMPL_ARRLIST_NAMED(ScenePtr, Scene*);
IMPL_ARRLIST_NAMED(WorldPtr, World*);
IMPL_ARRLIST_NAMED(ComponentStoragePtr, ComponentStorage*);
IMPL_ARRLIST_NAMED(SystemPtr, System*);
IMPL_ARRLIST_NAMED(StaticString, const char*);
IMPL_HASHMAP(int, BOOL, KeyMap, ez_hash_int);
IMPL_HASHMAP(EntityID, size_t, EntityMap, ez_hash_uint64_t);
IMPL_HASHMAP(size_t, ComponentStorage*, StorageMap, ez_hash_size_t);
IMPL_HASHMAP(EntityID, EntityID, Parents, ez_hash_uint64_t);
IMPL_HASHMAP(EntityID, ARRLIST_EntityID*, Children, ez_hash_uint64_t);
