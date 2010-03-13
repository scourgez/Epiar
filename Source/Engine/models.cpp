/**\file			models.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/models.h"
#include "Utilities/components.h"

/**\class Models
 * \brief Handles ship models. */

/**\brief Creates an empty Model object.
 */
Model::Model(): image(NULL),engine(NULL),mass(0.0){
	SetName("dead");
}

/**\brief Assignment operator (copy fields).
 */
Model& Model::operator=(const Model& other) {
	name = other.name;
	image = other.image;
	engine = other.engine;
	mass = other.mass;
	thrustOffset = other.thrustOffset;
	rotPerSecond = other.rotPerSecond;
	maxSpeed = other.maxSpeed;
	msrp = other.msrp;
	maxEnergyAbsorption = other.maxEnergyAbsorption;
	return *this;
}

/**\brief Creates a Model with the given parameters.
 * \param _name Name of the ship
 * \param _image Image of the ship
 * \param _engine Default Engine of the ship
 * \param _mass Mass of the ship
 * \param _thrustOffset For animation
 * \param _rotPerSecond Rotation per second
 * \param _maxSpeed Maximum speed
 * \param _maxEnergyAbsorption Maximum damage it can take
 * \param _msrp Price
 */
Model::Model( string _name, Image* _image, Engine* _engine, float _mass,
		short int _thrustOffset, float _rotPerSecond, float _maxSpeed, int _maxEnergyAbsorption, int _msrp) :
	image(_image),
	engine(_engine),
	mass(_mass),
	thrustOffset(_thrustOffset),
	rotPerSecond(_rotPerSecond),
	maxSpeed(_maxSpeed),
	msrp(_msrp),
	maxEnergyAbsorption(_maxEnergyAbsorption)
{
	SetName(_name);
	//((Component*)this)->SetName(_name);
}

/**\brief For parsing XML file into fields.
 */
bool Model::parserCB( string sectionName, string subName, string value ) {
	PPA_MATCHES( "name" ) {
		name = value;
	} else PPA_MATCHES( "image" ) {
		image = (Image*)Image::Get( value );
	} else PPA_MATCHES( "mass" ) {
		mass = (float)atof( value.c_str() );
	} else PPA_MATCHES( "rotationsPerSecond" ) {
		rotPerSecond = static_cast<float>(atof( value.c_str() ));
	} else PPA_MATCHES( "engine" ) {
		Engines *engines = Engines::Instance();

		engine = engines->GetEngine( value );

		if( engine == NULL ) {
			Log::Error( "Model parser could not find engine '%s'.", value.c_str() );
		}
	} else PPA_MATCHES( "thrustOffset" ) {
		thrustOffset = (short)atoi( value.c_str() );
	} else PPA_MATCHES( "maxSpeed" ) {
		maxSpeed = (float)atof( value.c_str() );
	} else PPA_MATCHES( "msrp" ) {
		msrp = atoi( value.c_str() );
	} else PPA_MATCHES( "maxEnergyAbsorption" ) {
		maxEnergyAbsorption = (short)atoi( value.c_str() );
	}
	// TODO This is a bad spot for this.
	if(image!=NULL && name!="dead"){ 
		Image::Store(name,(Resource*)image);
	}
	return true;
}

/**\brief Prints debugging information.
 */
void Model::_dbg_PrintInfo( void ) {
	if( mass <= 0.001 ){
		// Having an incorrect Mass can cause the Model to have NAN position which will cause it to disappear unexpectedly.
		Log::Error("Model %s does not have a valid Mass value (%f).",name.c_str(),mass);
	}
	if(image!=NULL && name!=""){ 
		cout<<"Storing Image for "<<name<<endl;
		Image::Store(name,(Resource*)image);
	} else { assert(0); }
}

/**\brief Converts the Model to an XML node.
 */
xmlNodePtr Model::ToXMLNode(string componentName) {
	char buff[256];
    xmlNodePtr section = xmlNewNode(NULL, BAD_CAST componentName.c_str());

	xmlNewChild(section, NULL, BAD_CAST "name", BAD_CAST this->GetName().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "image", BAD_CAST this->GetImage()->GetPath().c_str() );
	xmlNewChild(section, NULL, BAD_CAST "engine", BAD_CAST this->GetEngine()->GetName().c_str() );
	snprintf(buff, sizeof(buff), "%1.2f", this->GetMass() );
	xmlNewChild(section, NULL, BAD_CAST "mass", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%1.2f", this->GetRotationsPerSecond() );
	xmlNewChild(section, NULL, BAD_CAST "rotationsPerSecond", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetThrustOffset() );
	xmlNewChild(section, NULL, BAD_CAST "thrustOffset", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%1.1f", this->GetMaxSpeed() );
	xmlNewChild(section, NULL, BAD_CAST "maxSpeed", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->getMaxEnergyAbsorption() );
	xmlNewChild(section, NULL, BAD_CAST "maxEnergyAbsorption", BAD_CAST buff );
	snprintf(buff, sizeof(buff), "%d", this->GetMSRP() );
	xmlNewChild(section, NULL, BAD_CAST "msrp", BAD_CAST buff );

	return section;
}

/**\brief Retrieves the acceleration rate.
 */
float Model::GetAcceleration( void ) {
	if( engine ) {
		return( engine->GetForceOutput() / (float)mass );
	} else {
		return( 0. ); // no engine
	}
}

/**\brief Retrieves Flare animation.
 */
string Model::GetFlareAnimation( void ) {
	if( engine ) {
		return( engine->GetFlareAnimation() );
	} else {
		return string("Error");
	}
}

/**\brief Plays the Engine sound.
 */
void Model::PlayEngineThrust( float volume, Coordinate offset ){
	this->engine->thrustsound->SetVolume( volume );
	this->engine->thrustsound->PlayNoRestart( offset );
}

/**\fn Model::GetRotationsPerSecond()
 *  \brief Retrieves the rotation per second of the model.
 * \fn Model::GetMaxSpeed()
 *  \brief Retrieves the maximum speed
 * \fn Model::GetEngine()
 *  \brief Retrieves a pointer to the Engine
 * \fn Model::GetMass()
 *  \brief Retrieves the mass of the Model
 * \fn Model::GetImage()
 *  \brief Retrieves a pointer to the Image
 * \fn Model::GetThrustOffset()
 *  \brief Retrieves the offset of the thrust from center
 * \fn Model::getMaxEnergyAbsorption()
 *  \brief Retrieves the maximum energy it can absorb
 * \fn Model::GetMSRP()
 *  \brief Retrieves the price of the Model
 */


/**\class Models
 * \brief Collection of Model objects
 */
Models *Models::pInstance = 0; // initialize pointer

/**\brief Returns or creates the Model instance.
 * \return Pointer to the Model instance
 */
Models *Models::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Models; // create the sold instance
		pInstance->rootName = "models";
		pInstance->componentName = "model";
	}
	return( pInstance );
}

/**\fn Models::GetModel(string name)
 *  \brief Retrieves the Model by name
 * \fn Models::newComponent()
 *  \brief Creates a new Model
 * \fn Models::Models(const Models&)
 *  \brief An empty constructor.
 * \fn Models::operator= (const Models&)
 *  \brief An empty assignment operator.
 */
