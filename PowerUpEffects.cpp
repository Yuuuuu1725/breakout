#include "PowerUpEffects.h"
#include "Game.h"
#include "Ball.h"
#include "Paddle.h"

ExtendEffect::ExtendEffect(float dur) : timer(dur) {}
void ExtendEffect::Activate(Game& game) { game.ExtendPaddle(40); game.extendStackCount++; }
void ExtendEffect::Update(float dt, Game& game) { timer -= dt; }
void ExtendEffect::Deactivate(Game& game) { game.ShrinkPaddle(40); if(game.extendStackCount > 0) game.extendStackCount--; }
bool ExtendEffect::IsExpired() const { return timer <= 0; }

void MultiBallEffect::Activate(Game& game) { game.SpawnExtraBall(); }
bool MultiBallEffect::IsExpired() const { return true; } 

SlowEffect::SlowEffect(float dur, float mult) : timer(dur), targetMult(mult) {}
void SlowEffect::Activate(Game& game) { game.SetBallSpeedMult(targetMult); }
void SlowEffect::Update(float dt, Game& game) { timer -= dt; }
void SlowEffect::Deactivate(Game& game) { game.ResetBallSpeedMult(); }
bool SlowEffect::IsExpired() const { return timer <= 0; }

std::unique_ptr<PowerUpEffect> PowerUpFactory::Create(PowerUpType type, float extendDur, float slowDur, float slowMult) {
    switch (type) {
        case PowerUpType::EXTEND: return std::make_unique<ExtendEffect>(extendDur);
        case PowerUpType::MULTI:  return std::make_unique<MultiBallEffect>();
        case PowerUpType::SLOW:   return std::make_unique<SlowEffect>(slowDur, slowMult);
        default: return nullptr;
    }
}
