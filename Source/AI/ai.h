/**\file			ai.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef AI_H_
#define AI_H_

#include "Sprites/ship.h"

// Sprites have an AI object which is used to manipulate their attributes
// to run an AI simulation
class AI : public Ship {
	public:
		AI(string machine);
		void Update();
		void Draw();
		void Decide();
		void SetStateMachine(string _machine) { stateMachine = _machine; }
		void SetState(string _state)  { state = _state; }
		string GetStateMachine() { return stateMachine; }
		string GetState()  { return state; }

	private:
		string stateMachine;
		string state;
};

#endif /*AI_H_*/
