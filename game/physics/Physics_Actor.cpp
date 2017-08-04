/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod (http://www.thedarkmod.com/)
 
******************************************************************************/

#include "precompiled.h"
#pragma hdrstop



#include "../Game_local.h"

CLASS_DECLARATION( idPhysics_Base, idPhysics_Actor )
END_CLASS

/*
================
idPhysics_Actor::idPhysics_Actor
================
*/
idPhysics_Actor::idPhysics_Actor( void ) {
	clipModel = NULL;
	SetClipModelAxis();
	mass = 100.0f;
	invMass = 1.0f / mass;
	masterEntity = NULL;
	masterYaw = 0.0f;
	masterDeltaYaw = 0.0f;
	groundEntityPtr = NULL;

#ifdef MOD_WATERPHYSICS
	previousWaterLevel = waterLevel = WATERLEVEL_NONE;	// MOD_WATERPHYSICS
	waterType = 0;					// MOD_WATERPHYSICS
	waterLevelChanged = true;
	submerseFrame = 0;
	submerseTime = -1;
#endif		// MOD_WATERPHYSICS
}

/*
================
idPhysics_Actor::~idPhysics_Actor
================
*/
idPhysics_Actor::~idPhysics_Actor( void ) {
	if ( clipModel ) {
		delete clipModel;
		clipModel = NULL;
	}
}

/*
================
idPhysics_Actor::Save
================
*/
void idPhysics_Actor::Save( idSaveGame *savefile ) const {

	savefile->WriteClipModel( clipModel );
	savefile->WriteMat3( clipModelAxis );

	savefile->WriteFloat( mass );
	savefile->WriteFloat( invMass );

	savefile->WriteObject( masterEntity );
	savefile->WriteFloat( masterYaw );
	savefile->WriteFloat( masterDeltaYaw );

#ifdef MOD_WATERPHYSICS
	savefile->WriteInt( (int)waterLevel );	// MOD_WATERPHYSICS
	savefile->WriteInt((int)previousWaterLevel);
	savefile->WriteInt( waterType );		// MOD_WATERPHYSICS
	savefile->WriteBool(waterLevelChanged);
	savefile->WriteInt(submerseFrame);
	savefile->WriteInt(submerseTime);
#endif 		// MOD_WATERPHYSICS

	groundEntityPtr.Save( savefile );
}

/*
================
idPhysics_Actor::Restore
================
*/
void idPhysics_Actor::Restore( idRestoreGame *savefile ) {

	savefile->ReadClipModel( clipModel );
	savefile->ReadMat3( clipModelAxis );

	savefile->ReadFloat( mass );
	savefile->ReadFloat( invMass );

	savefile->ReadObject( reinterpret_cast<idClass *&>( masterEntity ) );
	savefile->ReadFloat( masterYaw );
	savefile->ReadFloat( masterDeltaYaw );

#ifdef MOD_WATERPHYSICS
	savefile->ReadInt( (int &)waterLevel );		// MOD_WATERPHYSICS
	savefile->ReadInt( (int &)previousWaterLevel );
	savefile->ReadInt( waterType );				// MOD_WATERPHYSICS
	savefile->ReadBool(waterLevelChanged);
	savefile->ReadInt(submerseFrame);
	savefile->ReadInt(submerseTime);
#endif 		// MOD_WATERPHYSICS

	groundEntityPtr.Restore( savefile );
}

/*
================
idPhysics_Actor::SetClipModelAxis
================
*/
void idPhysics_Actor::SetClipModelAxis( void ) {
	// align clip model to gravity direction
	if ( ( gravityNormal[2] == -1.0f ) || ( gravityNormal == vec3_zero ) ) {
		clipModelAxis.Identity();
	}
	else {
		clipModelAxis[2] = -gravityNormal;
		clipModelAxis[2].NormalVectors( clipModelAxis[0], clipModelAxis[1] );
		clipModelAxis[1] = -clipModelAxis[1];
	}

	if ( clipModel ) {
		clipModel->Link( gameLocal.clip, self, 0, clipModel->GetOrigin(), clipModelAxis );
	}
}

/*
================
idPhysics_Actor::GetGravityAxis
================
*/
const idMat3 &idPhysics_Actor::GetGravityAxis( void ) const {
	return clipModelAxis;
}

/*
================
idPhysics_Actor::GetMasterDeltaYaw
================
*/
float idPhysics_Actor::GetMasterDeltaYaw( void ) const {
	return masterDeltaYaw;
}

/*
================
idPhysics_Actor::GetGroundEntity
================
*/
idEntity *idPhysics_Actor::GetGroundEntity( void ) const {
	return groundEntityPtr.GetEntity();
}

/*
================
idPhysics_Actor::SetClipModel
================
*/
void idPhysics_Actor::SetClipModel( idClipModel *model, const float density, int id, bool freeOld ) {
	assert( self );
	assert( model );					// a clip model is required
	assert( model->IsTraceModel() );	// and it should be a trace model
	assert( density > 0.0f );			// density should be valid

	if ( clipModel && clipModel != model && freeOld ) {
		delete clipModel;
	}
	clipModel = model;
	clipModel->Link( gameLocal.clip, self, 0, clipModel->GetOrigin(), clipModelAxis );
}

/*
================
idPhysics_Actor::GetClipModel
================
*/
idClipModel *idPhysics_Actor::GetClipModel( int id ) const {
	return clipModel;
}

/*
================
idPhysics_Actor::GetNumClipModels
================
*/
int idPhysics_Actor::GetNumClipModels( void ) const {
	return 1;
}

/*
================
idPhysics_Actor::SetMass
================
*/
void idPhysics_Actor::SetMass( float _mass, int id ) {
	assert( _mass > 0.0f );
	mass = _mass;
	invMass = 1.0f / _mass;
}

/*
================
idPhysics_Actor::GetMass
================
*/
float idPhysics_Actor::GetMass( int id ) const {
	return mass;
}

/*
================
idPhysics_Actor::SetClipMask
================
*/
void idPhysics_Actor::SetContents( int contents, int id ) {
	clipModel->SetContents( contents );
}

/*
================
idPhysics_Actor::SetClipMask
================
*/
int idPhysics_Actor::GetContents( int id ) const {
	return clipModel->GetContents();
}

/*
================
idPhysics_Actor::GetBounds
================
*/
const idBounds &idPhysics_Actor::GetBounds( int id ) const {
	return clipModel->GetBounds();
}

/*
================
idPhysics_Actor::GetAbsBounds
================
*/
const idBounds &idPhysics_Actor::GetAbsBounds( int id ) const {
	return clipModel->GetAbsBounds();
}

/*
================
idPhysics_Actor::IsPushable
================
*/
bool idPhysics_Actor::IsPushable( void ) const {
	return ( masterEntity == NULL );
}

/*
================
idPhysics_Actor::GetOrigin
================
*/
const idVec3 &idPhysics_Actor::GetOrigin( int id ) const {
	return clipModel->GetOrigin();
}

/*
================
idPhysics_Player::GetAxis
================
*/
const idMat3 &idPhysics_Actor::GetAxis( int id ) const {
	return clipModel->GetAxis();
}

/*
================
idPhysics_Actor::SetGravity
================
*/
void idPhysics_Actor::SetGravity( const idVec3 &newGravity ) {
	if ( newGravity != gravityVector ) {
		idPhysics_Base::SetGravity( newGravity );
		SetClipModelAxis();
	}
}

/*
================
idPhysics_Actor::ClipTranslation
================
*/
void idPhysics_Actor::ClipTranslation( trace_t &results, const idVec3 &translation, const idClipModel *model ) const {
	if ( model ) {
		gameLocal.clip.TranslationModel( results, clipModel->GetOrigin(), clipModel->GetOrigin() + translation,
								clipModel, clipModel->GetAxis(), clipMask,
								model->Handle(), model->GetOrigin(), model->GetAxis() );
	}
	else {
		gameLocal.clip.Translation( results, clipModel->GetOrigin(), clipModel->GetOrigin() + translation,
								clipModel, clipModel->GetAxis(), clipMask, self );
	}
}

/*
================
idPhysics_Actor::ClipRotation
================
*/
void idPhysics_Actor::ClipRotation( trace_t &results, const idRotation &rotation, const idClipModel *model ) const {
	if ( model ) {
		gameLocal.clip.RotationModel( results, clipModel->GetOrigin(), rotation,
								clipModel, clipModel->GetAxis(), clipMask,
								model->Handle(), model->GetOrigin(), model->GetAxis() );
	}
	else {
		gameLocal.clip.Rotation( results, clipModel->GetOrigin(), rotation,
								clipModel, clipModel->GetAxis(), clipMask, self );
	}
}

/*
================
idPhysics_Actor::ClipContents
================
*/
int idPhysics_Actor::ClipContents( const idClipModel *model ) const {
	if ( model ) {
		return gameLocal.clip.ContentsModel( clipModel->GetOrigin(), clipModel, clipModel->GetAxis(), -1,
									model->Handle(), model->GetOrigin(), model->GetAxis() );
	}
	else {
		return gameLocal.clip.Contents( clipModel->GetOrigin(), clipModel, clipModel->GetAxis(), -1, NULL );
	}
}

/*
================
idPhysics_Actor::DisableClip
================
*/
void idPhysics_Actor::DisableClip( void ) {
	clipModel->Disable();
}

/*
================
idPhysics_Actor::EnableClip
================
*/
void idPhysics_Actor::EnableClip( void ) {
	clipModel->Enable();
}

/*
================
idPhysics_Actor::UnlinkClip
================
*/
void idPhysics_Actor::UnlinkClip( void ) {
	clipModel->Unlink();
}

/*
================
idPhysics_Actor::LinkClip
================
*/
void idPhysics_Actor::LinkClip( void ) {
	clipModel->Link( gameLocal.clip, self, 0, clipModel->GetOrigin(), clipModel->GetAxis() );
}

/*
================
idPhysics_Actor::EvaluateContacts
================
*/
bool idPhysics_Actor::EvaluateContacts( void ) {

	// get all the ground contacts
	ClearContacts();
	AddGroundContacts( clipModel );
	AddContactEntitiesForContacts();

	return ( contacts.Num() != 0 );
}

#ifdef MOD_WATERPHYSICS
/*
=============
idPhysics_Actor::SetWaterLevel
=============
*/
void idPhysics_Actor::SetWaterLevel( bool updateWaterLevelChanged ) {
	//
	// get waterlevel, accounting for ducking
	//
	// Remember the current water level
	previousWaterLevel = waterLevel;

	waterLevel = WATERLEVEL_NONE;
	waterType = 0;

	const idVec3& origin = this->GetOrigin();
	const idBounds& bounds = clipModel->GetBounds();

	// check at feet level
	idVec3 point = origin - ( bounds[0][2] + 1.0f ) * gravityNormal;
	int contents = gameLocal.clip.Contents( point, NULL, mat3_identity, -1, self );
	if ( contents & MASK_WATER ) {
		// sets water entity
		this->SetWaterLevelf();

		waterType = contents;
		waterLevel = WATERLEVEL_FEET;

		// check at waist level
		point = origin - ( bounds[1][2] - bounds[0][2] ) * 0.5f * gravityNormal;
		contents = gameLocal.clip.Contents( point, NULL, mat3_identity, -1, self );
		if ( contents & MASK_WATER ) {

			waterLevel = WATERLEVEL_WAIST;

			// check at head level
			// point = origin - ( bounds[1][2] - 1.0f ) * gravityNormal;

			// greebo: Changed the check for head level to test the eyeheight. This is enough
			// for letting AI drown or for the player to "feel" underwater.
			point = static_cast<idActor*>(self)->GetEyePosition();

			contents = gameLocal.clip.Contents( point, NULL, mat3_identity, -1, self );
			if ( contents & MASK_WATER ) {
				waterLevel = WATERLEVEL_HEAD;
			}
		}
	}
	else
		this->SetWater(NULL, 0.0f);

	if (updateWaterLevelChanged)
	{
		// Set the changed flag
		waterLevelChanged = (previousWaterLevel != waterLevel);

		if (waterLevel == WATERLEVEL_HEAD && waterLevelChanged)
		{
			submerseFrame = gameLocal.framenum;
			submerseTime = gameLocal.time;
		}
	}
}

/*
================
idPhysics_Actor::GetWaterLevel
================
*/
waterLevel_t idPhysics_Actor::GetWaterLevel( void ) const {
	return waterLevel;
}

/*
================
idPhysics_Actor::GetWaterType
================
*/
int idPhysics_Actor::GetWaterType( void ) const {
	return waterType;
}

int idPhysics_Actor::GetSubmerseTime() const
{
	return submerseTime;
}

#endif	// MOD_WATERPHYSICS
