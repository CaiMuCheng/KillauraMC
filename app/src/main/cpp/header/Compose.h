//
// Created by 35785 on 2023/7/2.
//

#ifndef KILLAURAMC_COMPOSE_H
#define KILLAURAMC_COMPOSE_H

#include <vector>
#include <string>

typedef int EntityUniqueID;

class Entity;

class MinecraftClient;

class Mob;

class Player;

class LocalPlayer;

class Level;

class GameMode;

enum class EntityType;

enum class DimensionId;

class AABB;


struct Vec3;


class Entity {
public:
    virtual ~Entity();

    virtual Vec3 const &getPos() const;

    EntityUniqueID *getUniqueID() const;

    virtual void remove();

    bool isRemoved() const;

    Level &getLevel();

    virtual bool killed(Entity*);

    void _updateOwnerChunk();
};

class MinecraftClient {
public:
    void onPlayerLoaded(Player &);
};

class Mob : public Entity {
public:
    virtual ~Mob();

    int getHealth() const;
};

class Player : public Mob {
public:
    virtual ~Player();

    virtual void attack(Entity &);

    virtual void setName(std::string const &);
};

class LocalPlayer : public Player {

};

class Level {
public:
    virtual ~Level();
    LocalPlayer* getLocalPlayer() const;
};

class GameMode {
public:
    virtual ~GameMode();
    virtual void tick();
    virtual void attack(Player&, Entity&);
};

enum class DimensionId : int
{
    OVERWORLD,
    NETHER,
    END
};

enum class EntityType : int {
    ITEM = 64,
    PRIMED_TNT,
    FALLING_BLOCK,
    EXPERIENCE_POTION = 68,
    EXPERIENCE_ORB,
    FISHINGHOOK = 77,
    ARROW = 80,
    SNOWBALL,
    THROWNEGG,
    PAINTING,
    LARGE_FIREBALL = 85,
    THROWN_POTION,
    LEASH_FENCE_KNOT = 88,
    BOAT = 90,
    LIGHTNING_BOLT = 93,
    SAMLL_FIREBALL,
    TRIPOD_CAMERA = 318,
    PLAYER,
    IRON_GOLEM = 788,
    SOWN_GOLEM,
    VILLAGER = 1807,
    CREEPER = 2849,
    SLIME = 2853,
    ENDERMAN,
    GHAST = 2857,
    LAVA_SLIME,
    BLAZE,
    WITCH = 2861,
    CHICKEN = 5898,
    COW,
    PIG,
    SHEEP,
    MUSHROOM_COW = 5904,
    RABBIT = 5906,
    SQUID = 10001,
    WOLF = 22286,
    OCELOT = 22294,
    BAT = 33043,
    PIG_ZOMBIE = 68388,
    ZOMBIE = 199456,
    ZOMBIE_VILLAGER = 199468,
    SPIDER = 264995,
    SILVERFISH = 264999,
    CAVE_SPIDER,
    MINECART_RIDEABLE = 524372,
    MINECART_HOPPER = 524384,
    MINECART_MINECART_TNT,
    MINECART_CHEST,
    SKELETON = 1116962,
    WITHER_SKELETON = 1116974,
    STRAY = 1116976,
    HORSE = 2119447,
    DONKEY,
    MULE,
    SKELETON_HORSE,
    ZOMBIE_HORSE
};

class EntityClassTree {
public:
    static bool isInstanceOf(Entity const &, EntityType);
};

struct Vec3 {
    float x, y, z;

    static const Vec3 ZERO;
    static const Vec3 ONE;

    static const Vec3 UNIT_X, UNIT_Y, UNIT_Z;
    static const Vec3 NEG_UNIT_X, NEG_UNIT_Y, NEG_UNIT_Z;

    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    Vec3() = default;

    float lengthSquared() const;

    float length() const;

    void normalize();

    void setLength(float newLength);

    Vec3 operator+(Vec3 const &other) const;

    void operator+=(Vec3 const &other);

    Vec3 operator-(Vec3 const &other) const;

    void operator-=(Vec3 const &other);

    Vec3 operator*(float factor) const;

    void operator*=(float factor);

    Vec3 operator/(float factor) const;

    void operator/=(float factor);

    Vec3 operator-() const;
};

#endif //KILLAURAMC_COMPOSE_H
