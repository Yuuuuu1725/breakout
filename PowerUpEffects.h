#ifndef POWERUP_EFFECTS_H
#define POWERUP_EFFECTS_H

#include <memory>

enum class PowerUpType { EXTEND, MULTI, SLOW };
class Game;

class PowerUpEffect {
public:
    virtual ~PowerUpEffect() = default;
    virtual void Activate(Game& game) = 0;
    virtual void Update(float dt, Game& game) = 0;
    virtual void Deactivate(Game& game) = 0;
    virtual bool IsExpired() const = 0;
};

class ExtendEffect : public PowerUpEffect {
private:
    float timer;
public:
    ExtendEffect(float dur);
    void Activate(Game& game) override;
    void Update(float dt, Game& game) override;
    void Deactivate(Game& game) override;
    bool IsExpired() const override;
};

class MultiBallEffect : public PowerUpEffect {
public:
    void Activate(Game& game) override;
    void Update(float dt, Game& game) override {}
    void Deactivate(Game& game) override {}
    bool IsExpired() const override;
};

class SlowEffect : public PowerUpEffect {
private:
    float timer;
    float targetMult;
public:
    SlowEffect(float dur, float mult);
    void Activate(Game& game) override;
    void Update(float dt, Game& game) override;
    void Deactivate(Game& game) override;
    bool IsExpired() const override;
};

class PowerUpFactory {
public:
    static std::unique_ptr<PowerUpEffect> Create(PowerUpType type, float extendDur, float slowDur, float slowMult);
};

#endif
