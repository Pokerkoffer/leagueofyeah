#pragma once
#include "SDK/PluginSDK.h"

class AIO
{
public:
	virtual void OnGameUpdate();
	virtual void OnRender();
	virtual void OnRealSpellCast(CastedSpell const& Args);
	virtual void OnSpellCast(CastedSpell const& Args);
	virtual bool OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);
	virtual void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	virtual void BeforeAttack(IUnit* Target);
	virtual void OnInterruptible(InterruptibleSpell const& Args);
	virtual void OnGapCloser(GapCloserSpell const& Args);
	virtual void OnLevelUp(IUnit* Source, int NewLevel);
	virtual void OnPlayAnimation(IUnit* Source, std::string const Args);
	virtual void OnExitVisible(IUnit* Source);
	virtual void OnTeleport(OnTeleportArgs* Args);
};