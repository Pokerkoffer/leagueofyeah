#pragma once
#include "AIO.h"

class BlitzCrank : public virtual Champion
{
public:
	BlitzCrank(IMenu* Parent, IUnit* Hero);
	~BlitzCrank(); // DeConstruct this nibba

	void Combo();
	void Harass();
	void LaneClear();

	void OnGameUpdate();
	void OnRender();
	void OnSpellCast(CastedSpell const& Args);
	void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	bool OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);

private:
	IMenu* BlitzCrankMenu;
	IMenu* Drawings;
	IMenu* Misc;
	IMenu* LaneClearMenu;
	IMenu* HarassMenu;
	IMenu* ComboMenu;
	IMenu* HookSettings;

	IMenuOption* HookMode;
	IMenuOption* AutoHook;
};