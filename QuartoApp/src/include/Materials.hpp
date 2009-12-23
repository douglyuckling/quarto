/**
 * @file Materials.hpp
 */

#include <Material.hpp>

using peek::Material;
using peek::Color;

static const Color boardSurfMaterialAmbient(0.15f, 0.05f, 0.0f);
static const Color boardSurfMaterialDiffuse(0.45f, 0.15f, 0.0f);
static const Color boardSurfMaterialSpecular(1.0f, 0.4f, 0.15f);
static const Color boardSurfMaterialEmission(0.0f, 0.0f, 0.0f);
static const float boardSurfMaterialShininess = 40.0f;

static const Material boardSurfMaterial(boardSurfMaterialAmbient, boardSurfMaterialDiffuse,
										boardSurfMaterialSpecular, boardSurfMaterialEmission,
										boardSurfMaterialShininess);

static const Color boardGrooveMaterialAmbient(0.1f, 0.05f, 0.0f);
static const Color boardGrooveMaterialDiffuse(0.3f, 0.15f, 0.0f);
static const Color boardGrooveMaterialSpecular(0.0f, 0.0f, 0.0f);
static const Color boardGrooveMaterialEmission(0.0f, 0.0f, 0.0f);
static const float boardGrooveMaterialShininess = 0.0f;

static const Material boardGrooveMaterial(boardGrooveMaterialAmbient, boardGrooveMaterialDiffuse,
									boardGrooveMaterialSpecular, boardGrooveMaterialEmission,
									boardGrooveMaterialShininess);

static const Color whitePieceMaterialAmbient(0.4f, 0.2f, 0.0f);
static const Color whitePieceMaterialDiffuse(0.8f, 0.4f, 0.0f);
static const Color whitePieceMaterialSpecular(1.0f, 0.7f, 0.3f);
static const Color whitePieceMaterialEmission(0.0f, 0.0f, 0.0f);
static const float whitePieceMaterialShininess = 40.0f;

static const Material whitePieceMaterial(whitePieceMaterialAmbient, whitePieceMaterialDiffuse,
										 whitePieceMaterialSpecular, whitePieceMaterialEmission,
										 whitePieceMaterialShininess);

static const Color blackPieceMaterialAmbient(0.1f, 0.05f, 0.0f);
static const Color blackPieceMaterialDiffuse(0.3f, 0.15f, 0.0f);
static const Color blackPieceMaterialSpecular(1.0f, 0.575f, 0.25f);
static const Color blackPieceMaterialEmission(0.0f, 0.0f, 0.0f);
static const float blackPieceMaterialShininess = 40.0f;

static const Material blackPieceMaterial(blackPieceMaterialAmbient, blackPieceMaterialDiffuse,
										 blackPieceMaterialSpecular, blackPieceMaterialEmission,
										 blackPieceMaterialShininess);
