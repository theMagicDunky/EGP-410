/*main.cpp
 *	Editor for pathfinding project
 *
 *	Dean Lawson
 *	Champlain College
 *	2011
 *
 */
#define ALLEGRO_USE_CONSOLE //allows the console to be used
#include <allegro5/allegro.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>

#include "Game.h"
#include "Editor.h"
#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include "Sprite.h"
#include "Timer.h"
#include "Grid.h"
#include "GridVisualizer.h"
#include "PerformanceTracker.h"
#include "MemoryTracker.h"

int main(void)
{
	//create the Editor
	gpGame = new Editor();

	gpGame->init();

	bool shouldExit = false;

	while( !shouldExit )
	{
		//get current keyboard state
		ALLEGRO_KEYBOARD_STATE keyState;
		al_get_keyboard_state( &keyState );

		//if escape key was down then exit the loop
		if( al_key_down( &keyState, ALLEGRO_KEY_ESCAPE ) )
		{
			gpGame->markForExit();
		}
		else if( al_key_down( &keyState, ALLEGRO_KEY_S ) )
		{
			Editor* pEditor = dynamic_cast<Editor*>(gpGame);
			if( pEditor != NULL )
			{
				int levelNumber = gpGame->getCurrentLevelIndex() + 1;
				ofstream theStream("../assets/Level" + std::to_string(levelNumber) + ".tmx");
				pEditor->saveGrid(theStream);
				theStream.close();
				cout << "Grid saved to ../assets/Level" + std::to_string(levelNumber) + ".tmx!\n";
				Sleep(1000);//very bogus
			}
		}
		else if( al_key_down( &keyState, ALLEGRO_KEY_L ) )
		{
			/*Editor* pEditor = dynamic_cast<Editor*>(gpGame);
			if( pEditor != NULL )
			{
				ifstream theStream(FILE_NAME);
				pEditor->loadGrid(theStream);
				theStream.close();
				pEditor->getGridVisualizer()->setModified();
				cout << "Grid loaded!\n";
				Sleep(1000);//very bogus
			}*/
		}

		gpGame->beginLoop();
		gpGame->processLoop();
		shouldExit = gpGame->endLoop();
	}

	//cleanup
	gpGame->cleanup();
	delete gpGame;
	gMemoryTracker.reportAllocations( std::cout );

	system("pause");

	return 0;
}

