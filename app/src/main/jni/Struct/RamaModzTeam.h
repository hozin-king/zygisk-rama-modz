#pragma once
#define offset_OnCalcDamage                                                    \
(uintptr_t)Il2CppGetMethodOffset(                                            \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("HBIBDMMOOOK"), OBFUSCATE("LHNJLFOINDO"), 1)
#define offset_GetHeadCollider_Silent                                          \
(uintptr_t)Il2CppGetMethodOffset(                                            \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("Player"), OBFUSCATE("get_HeadCollider"), 0)
#define offset_GetGameObject                                                   \
(uintptr_t)Il2CppGetMethodOffset(                                            \
OBFUSCATE("UnityEngine.CoreModule.dll"), OBFUSCATE("UnityEngine"),       \
OBFUSCATE("Component"), OBFUSCATE("get_gameObject"), 0)
#define offset_HitObject                                                       \
(uintptr_t)Il2CppGetFieldOffset(                                             \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("CGKJLKPMGDJ"), OBFUSCATE("JHAINOBIFMA"))
#define offset_HitCollider                                                     \
(uintptr_t)Il2CppGetFieldOffset(                                             \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("CGKJLKPMGDJ"), OBFUSCATE("JINDNOFABKB"))
#define offset_HitGroup                                                        \
(uintptr_t)Il2CppGetFieldOffset(                                             \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("CGKJLKPMGDJ"), OBFUSCATE("DKPDBLLJEAA"))
#define offset_HitLocation                                                     \
(uintptr_t)Il2CppGetFieldOffset(                                             \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("CGKJLKPMGDJ"), OBFUSCATE("HPCOJHGFEEG"))
#define offset_HitNormal                                                       \
(uintptr_t)Il2CppGetFieldOffset(                                             \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("CGKJLKPMGDJ"), OBFUSCATE("DOJDNHCGFGK"))
#define offset_RayDir                                                          \
(uintptr_t)Il2CppGetFieldOffset(                                             \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("CGKJLKPMGDJ"), OBFUSCATE("HLDECMJFKJK"))
#define offset_IgnoreHappens                                                   \
(uintptr_t)Il2CppGetFieldOffset(                                             \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("CGKJLKPMGDJ"), OBFUSCATE("EFMBNPNFGHM"))
#define offset_ViewBlocked                                                     \
(uintptr_t)Il2CppGetFieldOffset(                                             \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("CGKJLKPMGDJ"), OBFUSCATE("CEOKOILBODN"))
#define offset_SpecialHitType                                                  \
(uintptr_t)Il2CppGetFieldOffset(                                             \
OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),             \
OBFUSCATE("CGKJLKPMGDJ"), OBFUSCATE("CLOLKHNMNEK"))

bool AimSilent = false;
static bool g_BypassInstalled = false;

static void *GetHeadColliderSilent(void *player) {
  if (!player) return nullptr;
  void *(*_GetHead)(void *) = (void *(*)(void *))(offset_GetHeadCollider_Silent);
  return _GetHead(player);
}

static void *get_gameObject_Silent(void *component) {
  if (!component) return nullptr;
  void *(*_GetObj)(void *) = (void *(*)(void *))(offset_GetGameObject);
  return _GetObj(component);
}

static bool IsEnemyVisibleSilent(void *enemy) {
  if (!enemy) return false;

  void *hitObj = NULL;
  Vector3 cameraPos = Transform_GetPosition(Component_GetTransform(Camera_main()));
  Vector3 targetPos = Transform_GetPosition(Component_GetTransform(Player_GetHeadCollider(enemy)));

  return !Physics_Raycast(cameraPos, targetPos, 12, &hitObj);
}

static void *GetClosestEnemySilent() {
  float minAngle = 9999.0f;
  void *targetEnemy = nullptr;

  void *match = Curent_Match();  
  if (!match) return nullptr;  

  void *localPlayer = GetLocalPlayer(match);  
  if (!localPlayer) return nullptr;  

  void *camera = Camera_main();  
  if (!camera) return nullptr;  

  Vector3 myPos = getPosition(localPlayer);  
  Vector3 camFwd = GetForward(Component_GetTransform(camera));  

  MyDictionary* dict = *(MyDictionary**)((uintptr_t)match + ListPlayer);  
  if (!dict || !dict->entries) return nullptr;  

  int total = dict->count;  
  if (total <= 0 || total > 512) return nullptr;  

  for (int i = 0; i < total; i++) {  
      void *enemy = dict->entries->vector[i].value;  
      if (!enemy || enemy == localPlayer) continue;  
      if (get_isLocalTeam(enemy)) continue;  
      if (get_IsDieing(enemy)) continue;  
      if (get_MaxHP(enemy) <= 0) continue;  

      Vector3 enemyPos = getPosition(enemy);  
      float dist = Vector3::Distance(myPos, enemyPos);  
      if (dist > 1000.0f) continue;  

      Vector3 screenPos = WorldToScreenPoint(camera, enemyPos);  
      if (screenPos.z < 1) continue;  

      if (!IsEnemyVisibleSilent(enemy)) continue;  

      Vector3 dir = Vector3::Normalized(enemyPos - myPos);  
      float angle = Vector3::Angle(dir, camFwd) * 100.0f;  

      if (angle < minAngle) {  
          minAngle = angle;  
          targetEnemy = enemy;  
      }  
  }  

  return targetEnemy;
}

void (*orig_OnCalcDamage)(void *weapon, void *hitInfo);

void hook_OnCalcDamage(void *weapon, void *hitInfo) {
  if (!AimSilent) {
    if (orig_OnCalcDamage) orig_OnCalcDamage(weapon, hitInfo);
    return;
  }

  if (!weapon || !hitInfo) {  
    if (orig_OnCalcDamage) orig_OnCalcDamage(weapon, hitInfo);  
    return;  
  }  
    
  void *match = Curent_Match();  
  if (!match) {  
    orig_OnCalcDamage(weapon, hitInfo);  
    return;  
  }  
    
  void *localPlayer = GetLocalPlayer(match);  
  if (!localPlayer) {  
    orig_OnCalcDamage(weapon, hitInfo);  
    return;  
  }  
    
  void *target = GetClosestEnemySilent();  
  if (!target) {  
    orig_OnCalcDamage(weapon, hitInfo);  
    return;  
  }  
    
  if (get_IsDieing(target) || get_isLocalTeam(target)) {  
    orig_OnCalcDamage(weapon, hitInfo);  
    return;  
  }  
    
  void *headCollider = GetHeadColliderSilent(target);  
  if (!headCollider) {  
    orig_OnCalcDamage(weapon, hitInfo);  
    return;  
  }  
    
  void *headGameObject = get_gameObject_Silent(headCollider);  
  if (!headGameObject) {  
    orig_OnCalcDamage(weapon, hitInfo);  
    return;  
  }  
    
  Vector3 enemyHeadPos = GetHeadPosition(target);  
  Vector3 myPos = getPosition(localPlayer);  
  Vector3 direction = Vector3::Normalized(enemyHeadPos - myPos);  

  // Modify hitInfo for Headshot
  *(void **)((uintptr_t)hitInfo + offset_HitObject) = headGameObject;
  *(void **)((uintptr_t)hitInfo + offset_HitCollider) = headCollider;
  *(int *)((uintptr_t)hitInfo + offset_HitGroup) = 1; // Head = 1
  *(Vector3 *)((uintptr_t)hitInfo + offset_HitLocation) = enemyHeadPos;
  *(Vector3 *)((uintptr_t)hitInfo + offset_HitNormal) = direction;
  *(Vector3 *)((uintptr_t)hitInfo + offset_RayDir) = direction;
  *(bool *)((uintptr_t)hitInfo + offset_IgnoreHappens) = false;
  *(bool *)((uintptr_t)hitInfo + offset_ViewBlocked) = false;
  *(uint8_t *)((uintptr_t)hitInfo + offset_SpecialHitType) = 0;

  if (orig_OnCalcDamage) orig_OnCalcDamage(weapon, hitInfo);
}

void InitBypass(const char* gameVer) {
  if (g_BypassInstalled) return;

  // Nama obfuscated GANTI tiap build OB — tambah baris baru saat dump OB baru tersedia.
  // versionPrefix: awalan versionName game ("" = cocok semua, jadi fallback).
  struct ObfCandidate { const char* versionPrefix; const char* klass; const char* method; };
  static const ObfCandidate candidates[] = {
    {"", "HBIBDMMOOOK", "LHNJLFOINDO"}, // OB55
  };

  uintptr_t addr = 0;
  for (unsigned i = 0; i < sizeof(candidates)/sizeof(candidates[0]); i++) {
    const char* pre = candidates[i].versionPrefix;
    if (pre[0]) {
      if (!gameVer) continue;
      const char* a = gameVer; const char* b = pre;
      while (*b && *a == *b) { a++; b++; }
      if (*b) continue; // prefix tidak cocok
    }
    uintptr_t cand = (uintptr_t)Il2CppGetMethodOffset(
        OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"),
        candidates[i].klass, candidates[i].method, 1);
    if (cand && cand >= 0x100000) { addr = cand; break; }
  }
  if (!addr) return;

  if (DobbyHook((void *)addr, (void *)hook_OnCalcDamage, (void **)&orig_OnCalcDamage) != 0) return;
  g_BypassInstalled = true;
}

