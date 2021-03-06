#include "MasterYi.h"

MasterYi::~MasterYi() // We dont want our script to stay around
{
	MasterYiMenu->Remove();
}




/*
TODO LIST
Add Item Usage
Add R Usage
Add combo
Add W AA reset
*/

MasterYi::MasterYi(IMenu* Parent, IUnit* Hero) :Champion(Parent, Hero)
{
	auto SummonerSpell1 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot1);
	auto SummonerSpell2 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot2);
	if (Q == nullptr)
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
		Q->SetOverrideRange(600);
	}
	// W ability
	if (W == nullptr)
	{
		W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	}
	// E ability
	if (E == nullptr)
	{
		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	}
	// R ability
	if (R == nullptr)
	{
		R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
	}
	if (Smite == nullptr) // lol they're not playing right if they didnt take smite hehexd
	{
		if (strstr(SummonerSpell1, "SummonerSmite"))
		{
			Smite = GPluginSDK->CreateSpell2(kSummonerSlot1, kTargetCast, false, false, kCollidesWithNothing);
		}
		if (strstr(SummonerSpell2, "SummonerSmite"))
		{
			Smite = GPluginSDK->CreateSpell2(kSummonerSlot2, kTargetCast, false, false, kCollidesWithNothing);
		}
	}
	MasterYiMenu = GPluginSDK->AddMenu("Master Yi");
	JungleMenu = MasterYiMenu->AddMenu("Jungle Clear");
	QJung = JungleMenu->CheckBox("Use Q", true);
	EJung = JungleMenu->CheckBox("Use E", true);
	SmiteJung = JungleMenu->CheckBox("Use Smite", true);
	HealJung = JungleMenu->CheckBox("Heal if Low", true);
	LowHP = JungleMenu->AddInteger("Minimum Health", 1, 99, 10);
	JungKey = JungleMenu->AddKey("Jungle Clear Key", 90);

	LaneClearMenu = MasterYiMenu->AddMenu("Lane Clear");
	QLane = LaneClearMenu->CheckBox("Use Q", true);
	ELane = LaneClearMenu->CheckBox("Use E", false);

	Drawings = MasterYiMenu->AddMenu("Drawings");
	DrawQ = Drawings->CheckBox("Draw Q", true);
	VisibleOnly = Drawings->CheckBox("Only Draw Ready", true);


}

void MasterYi::JungleClear()
{
	for (auto pCreep : GEntityList->GetAllMinions(false, false, true))
	{
		if (!pCreep->IsDead() && pCreep != nullptr)
		{

			auto targetCreep = GTargetSelector->FindTarget(LowestHealthPriority, PhysicalDamage, 600);
			float flDistance = targetCreep->ServerPosition().DistanceTo(GEntityList->Player()->GetPosition());
			if (flDistance < 500 && Q->IsReady() && QJung->Enabled())
			{
				Q->CastOnTarget(targetCreep, kHitChanceMedium);
			}
			if (flDistance < 200 && E->IsReady() && EJung->Enabled())
			{
				E->CastOnPlayer();
			}
			if (GEntityList->Player()->HealthPercent() < LowHP->GetInteger() && W->IsReady() && HealJung->Enabled())
			{
				W->CastOnPlayer();
			}
			if (SmiteJung->Enabled() && Smite != nullptr)
			{
				if (strstr(pCreep->GetObjectName(), "Red") || strstr(pCreep->GetObjectName(), "Blue") || strstr(pCreep->GetObjectName(), "Baron") || strstr(pCreep->GetObjectName(), "Rift") || strstr(pCreep->GetObjectName(), "Dragon"))
				{
					if (pCreep->GetHealth() <= GDamage->GetSummonerSpellDamage(GEntityList->Player(), pCreep, kSummonerSpellSmite))
					{
						Smite->CastOnUnit(pCreep);
					}
				}
			}
		}
	}
}

void MasterYi::LaneClear()
{
	for (auto pMinion : GPluginSDK->GetEntityList()->GetAllMinions(false, true, false))
	{
		if (pMinion = nullptr)
			continue;
		if(pMinion->IsDead())
			continue;

		auto minion = GTargetSelector->FindTarget(ClosestPriority, PhysicalDamage, Q->Range()); // We use closest so we dont get fucked over by any possible enemy laner
		float flDistance = minion->ServerPosition().DistanceTo(GEntityList->Player()->GetPosition());
		if (flDistance > Q->Range())
			continue;
		std::vector<IUnit*> Close = GetMinionsNearby(false, true, false, Q->Range());
		if (Q->IsReady() && QLane->Enabled() && Close.size() >= 3)
			Q->CastOnUnit(minion);

		if (E->IsReady() && ELane)
		{	
			if (Close.size() >= 2)
			{
				E->CastOnPlayer();
			}
		}
	}
}

void MasterYi::Combo()
{
	for (auto pEnemy : GPluginSDK->GetEntityList()->GetAllHeros(false, true))
	{
		if(pEnemy == nullptr || pEnemy->IsDead())
			continue;
		IUnit* enemy = nullptr;
		if(Q->IsReady())
			 enemy = GTargetSelector->FindTarget(LowestHealthPriority, PhysicalDamage, Q->Range());
		else if(!Q->IsReady())
			 enemy = GTargetSelector->FindTarget(LowestHealthPriority, PhysicalDamage, GEntityList->Player()->AttackRange());

		float flDistance = enemy->ServerPosition().DistanceTo(GEntityList->Player()->GetPosition());
		if (enemy)
		{
			if(flDistance > 800)
				continue;
			if (R->IsReady())
				R->CastOnPlayer();
			if (E->IsReady())
				E->CastOnPlayer();
			if (Q->IsReady() && flDistance <= Q->Range())
				Q->CastOnUnit(enemy);

			if (GEntityList->Player()->HealthPercent() < 10 && !GUtility->IsPositionUnderTurret(GEntityList->Player()->GetPosition(), false, true))
				W->CastOnPlayer();

		}
	}
}

void MasterYi::Harass()
{
	for (auto pMinions : GEntityList->GetAllMinions(false, true, false))
	{
		for (auto pEnemy : GEntityList->GetAllHeros(false, true))
		{
			if (pEnemy == nullptr || pMinions == nullptr)
				continue;
			if(pEnemy->IsDead() || pMinions->IsDead())
			continue;

			auto minion = GTargetSelector->FindTarget(ClosestPriority, PhysicalDamage, Q->Range());
			auto enemy = GTargetSelector->FindTarget(LowestHealthPriority, PhysicalDamage, Q->Range());
			float flDistance = enemy->ServerPosition().DistanceTo(GEntityList->Player()->GetPosition());
			std::vector<IUnit*> Close = GetMinionsNearby(false, true, false, Q->Range());
			if (Close.size() <= 3 && flDistance < Q->Range() && Q->IsReady())
			{
				Q->CastOnUnit(minion);
			}
		}
	}
}

void MasterYi::Drawing()
{
	if (!VisibleOnly->Enabled() && DrawQ->Enabled())
	{
		GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), Q->Radius());
	}
	else if (DrawQ->Enabled() && Q->IsReady())
	{
		GPluginSDK->GetRenderer()->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 255, 255), Q->Radius());
	}
}

void MasterYi::OnGameUpdate()
{
	if (GPluginSDK->GetOrbwalking()->GetOrbwalkingMode() == kModeCombo)
		MasterYi::Combo();															// Combo
	if (GPluginSDK->GetOrbwalking()->GetOrbwalkingMode() == kModeMixed)
		MasterYi::Harass();															// Harass
	if (GPluginSDK->GetOrbwalking()->GetOrbwalkingMode() == kModeLaneClear)
		MasterYi::LaneClear();														// Lane Clear
	if (GUtility->IsKeyDown(JungKey->GetInteger()))
		MasterYi::JungleClear();													// Jungle Clear
}

void MasterYi::OnRender()
{
	if (!GEntityList->Player()->IsDead())
		Drawing();
}

void MasterYi::OnSpellCast(CastedSpell const& Args)
{

}
bool MasterYi::OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition)
{
	return true;
}

void MasterYi::OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{

}

