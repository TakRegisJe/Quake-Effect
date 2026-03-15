#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"

class rvWeaponMachinegun : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponMachinegun );

	rvWeaponMachinegun ( void );

	virtual void		Spawn				( void );
	virtual void		Think				( void );
	void				Save				( idSaveGame *savefile ) const;
	void				Restore				( idRestoreGame *savefile );
	void					PreSave				( void );
	void					PostSave			( void );

protected:

	float				spreadZoom;
	bool				fireHeld;

	bool				UpdateFlashlight	( void );
	void				Flashlight			( bool on );

private:

	//MOD - overheat system variables replacing ammo
	float				heatLevel;
	float				maxHeat;
	float				heatPerShot;
	float				cooldownRate;
	bool				overheated;
	//MOD-END

	stateResult_t		State_Idle			( const stateParms_t& parms );
	stateResult_t		State_Fire			( const stateParms_t& parms );
	//MOD - replaced State_Reload with State_Overheat
	stateResult_t		State_Overheat		( const stateParms_t& parms );
	//MOD-END
	stateResult_t		State_Flashlight	( const stateParms_t& parms );

	CLASS_STATES_PROTOTYPE ( rvWeaponMachinegun );
};

CLASS_DECLARATION( rvWeapon, rvWeaponMachinegun )
END_CLASS

/*
================
rvWeaponMachinegun::rvWeaponMachinegun
================
*/
rvWeaponMachinegun::rvWeaponMachinegun ( void ) {
}

/*
================
rvWeaponMachinegun::Spawn
================
*/
void rvWeaponMachinegun::Spawn ( void ) {
	spreadZoom = spawnArgs.GetFloat ( "spreadZoom" );
	fireHeld   = false;

	//MOD - initialize overheat variables from def
	heatLevel    = 0.0f;
	maxHeat      = spawnArgs.GetFloat( "maxHeat",     "100" );
	heatPerShot  = spawnArgs.GetFloat( "heatPerShot", "10"  );
	cooldownRate = spawnArgs.GetFloat( "cooldownRate", "25"  );
	overheated   = false;
	//MOD-END

	SetState ( "Raise", 0 );

	Flashlight ( owner->IsFlashlightOn() );
}

/*
================
rvWeaponMachinegun::Save
================
*/
void rvWeaponMachinegun::Save ( idSaveGame *savefile ) const {
	savefile->WriteFloat ( spreadZoom );
	savefile->WriteBool ( fireHeld );
	//MOD - save overheat state
	savefile->WriteFloat ( heatLevel );
	savefile->WriteFloat ( maxHeat );
	savefile->WriteFloat ( heatPerShot );
	savefile->WriteFloat ( cooldownRate );
	savefile->WriteBool  ( overheated );
	//MOD-END
}

/*
================
rvWeaponMachinegun::Restore
================
*/
void rvWeaponMachinegun::Restore ( idRestoreGame *savefile ) {
	savefile->ReadFloat ( spreadZoom );
	savefile->ReadBool ( fireHeld );
	//MOD - restore overheat state
	savefile->ReadFloat ( heatLevel );
	savefile->ReadFloat ( maxHeat );
	savefile->ReadFloat ( heatPerShot );
	savefile->ReadFloat ( cooldownRate );
	savefile->ReadBool  ( overheated );
	//MOD-END
}

/*
================
rvWeaponMachinegun::PreSave
================
*/
void rvWeaponMachinegun::PreSave ( void ) {
}

/*
================
rvWeaponMachinegun::PostSave
================
*/
void rvWeaponMachinegun::PostSave ( void ) {
}


/*
================
rvWeaponMachinegun::Think
================
*/
void rvWeaponMachinegun::Think()
{
	rvWeapon::Think();

	//MOD - drain heat every frame; clear overheat lockout when fully cooled
	if ( heatLevel > 0.0f ) {
		heatLevel -= cooldownRate * MS2SEC( gameLocal.msec );
		if ( heatLevel < 0.0f ) {
			heatLevel = 0.0f;
		}
	}
	if ( overheated && heatLevel <= 0.0f ) {
		overheated = false;
	}
	//MOD-END

	if ( zoomGui && owner == gameLocal.GetLocalPlayer( ) ) {
		zoomGui->SetStateFloat( "playerYaw", playerViewAxis.ToAngles().yaw );
	}
}

/*
================
rvWeaponMachinegun::UpdateFlashlight
================
*/
bool rvWeaponMachinegun::UpdateFlashlight ( void ) {
	if ( !wsfl.flashlight ) {
		return false;
	}

	SetState ( "Flashlight", 0 );
	return true;
}

/*
================
rvWeaponMachinegun::Flashlight
================
*/
void rvWeaponMachinegun::Flashlight ( bool on ) {
	owner->Flashlight ( on );

	if ( on ) {
		viewModel->ShowSurface ( "models/weapons/blaster/flare" );
		worldModel->ShowSurface ( "models/weapons/blaster/flare" );
	} else {
		viewModel->HideSurface ( "models/weapons/blaster/flare" );
		worldModel->HideSurface ( "models/weapons/blaster/flare" );
	}
}

/*
===============================================================================

	States

===============================================================================
*/

CLASS_STATES_DECLARATION ( rvWeaponMachinegun )
	STATE ( "Idle",			rvWeaponMachinegun::State_Idle )
	STATE ( "Fire",			rvWeaponMachinegun::State_Fire )
	//MOD - replaced Reload state with Overheat state
	STATE ( "Overheat",		rvWeaponMachinegun::State_Overheat )
	//MOD-END
	STATE ( "Flashlight",	rvWeaponMachinegun::State_Flashlight )
END_CLASS_STATES

/*
================
rvWeaponMachinegun::State_Idle
================
*/
stateResult_t rvWeaponMachinegun::State_Idle( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};
	switch ( parms.stage ) {
		case STAGE_INIT:
			//MOD - removed ammo check; weapon is always ready unless overheated
			SetStatus ( WP_READY );
			//MOD-END
			PlayCycle( ANIMCHANNEL_ALL, "idle", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );

		case STAGE_WAIT:
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			if ( UpdateFlashlight ( ) ) {
				return SRESULT_DONE;
			}
			if ( fireHeld && !wsfl.attack ) {
				fireHeld = false;
			}
			//MOD - replaced ammo/clip checks with overheat check
			if ( overheated ) {
				SetState ( "Overheat", 0 );
				return SRESULT_DONE;
			}
			if ( !fireHeld && gameLocal.time > nextAttackTime && wsfl.attack ) {
				SetState ( "Fire", 0 );
				return SRESULT_DONE;
			}
			//MOD-END
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponMachinegun::State_Fire
================
*/
stateResult_t rvWeaponMachinegun::State_Fire ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( wsfl.zoom ) {
				nextAttackTime = gameLocal.time + (altFireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
				Attack ( true, 1, spreadZoom, 0, 1.0f );
				fireHeld = true;
			} else {
				nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
				Attack ( false, 1, spread, 0, 1.0f );
			}
			//MOD - accumulate heat per shot; lock out if max heat reached
			heatLevel += heatPerShot;
			if ( heatLevel >= maxHeat ) {
				heatLevel  = maxHeat;
				overheated = true;
			}
			//MOD-END
			PlayAnim ( ANIMCHANNEL_ALL, "fire", 0 );
			return SRESULT_STAGE ( STAGE_WAIT );

		case STAGE_WAIT:
			//MOD - go to overheat state immediately if heat maxed out
			if ( overheated ) {
				SetState ( "Overheat", 0 );
				return SRESULT_DONE;
			}
			//MOD-END
			if ( !fireHeld && wsfl.attack && gameLocal.time >= nextAttackTime && !wsfl.lowerWeapon ) {
				SetState ( "Fire", 0 );
				return SRESULT_DONE;
			}
			if ( AnimDone ( ANIMCHANNEL_ALL, 0 ) ) {
				SetState ( "Idle", 0 );
				return SRESULT_DONE;
			}
			if ( UpdateFlashlight ( ) ) {
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

//MOD - new State_Overheat replaces State_Reload; blocks firing until heat drains to 0
/*
================
rvWeaponMachinegun::State_Overheat
  Locks out firing while the gun cools. Think() drains heatLevel
  and clears the overheated flag automatically.
================
*/
stateResult_t rvWeaponMachinegun::State_Overheat ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};
	switch ( parms.stage ) {
		case STAGE_INIT:
			SetStatus ( WP_OUTOFAMMO );
			PlayCycle ( ANIMCHANNEL_ALL, "idle", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );

		case STAGE_WAIT:
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			if ( !overheated ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}
//MOD-END

/*
================
rvWeaponMachinegun::State_Flashlight
================
*/
stateResult_t rvWeaponMachinegun::State_Flashlight ( const stateParms_t& parms ) {
	enum {
		FLASHLIGHT_INIT,
		FLASHLIGHT_WAIT,
	};
	switch ( parms.stage ) {
		case FLASHLIGHT_INIT:
			SetStatus ( WP_FLASHLIGHT );
			// Wait for the flashlight anim to play
			PlayAnim( ANIMCHANNEL_ALL, "flashlight", 0 );
			return SRESULT_STAGE ( FLASHLIGHT_WAIT );

		case FLASHLIGHT_WAIT:
			if ( !AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				return SRESULT_WAIT;
			}

			if ( owner->IsFlashlightOn() ) {
				Flashlight ( false );
			} else {
				Flashlight ( true );
			}

			SetState ( "Idle", 4 );
			return SRESULT_DONE;
	}
	return SRESULT_ERROR;
}
