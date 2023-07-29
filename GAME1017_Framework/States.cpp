#include <iostream>
#include "States.h"
#include "StateManager.h"
#include "Engine.h"
// GameObject includes.
#include "Primitives.h"
#include "Button3.h"
#include "TiledLevel.h"
#include "Player.h"
// Remaining managers.
#include "CollisionManager.h"
#include "EventManager.h"
#include "RenderManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "FontManager.h"
#include "PathManager.h"
#include <iostream>
#define PFREFRESH 30
using namespace std;

void State::Update()
{
	for (auto const& i : m_objects)
	{
		i.second->Update();
		if (STMA::StateChanging()) return;
	}
}

void State::Render()
{
	for (auto const& i : m_objects)
		i.second->Render();
	if (dynamic_cast<GameState*>(this) && dynamic_cast<PauseState*>(STMA::GetStates().back())) 
		return; // If GameState is rendering but PauseState is the current state, return.
	SDL_RenderPresent(REMA::GetRenderer());
}

void State::Exit()
{
	for (auto& i : m_objects)
	{
		delete i.second;
		i.second = nullptr;
	}
	m_objects.clear();
	m_objects.shrink_to_fit();
}

void State::AddChild(std::string key, GameObject* object)
{
	m_objects.push_back(pair<string, GameObject*>(key, object));
}

GameObject* State::GetChild(const std::string& key)
{
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[&](const std::pair<std::string, GameObject*>& element)
		{
			return element.first == key;
		});
	if (it != m_objects.end())
		return it->second;
	else return nullptr;
}

template<class T>
T State::GetChild(const std::string& key)
{
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[&](const std::pair<std::string, GameObject*>& element)
		{
			return element.first == key;
		});
	if (it != m_objects.end())
	{
		return static_cast<T>(it->second); // Downcasting now done here!
	}
	else return nullptr;
}

void State::RemoveChild(const std::string& key)
{
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[&](const std::pair<std::string, GameObject*>& element)
		{
			return element.first == key;
		});
	if (it != m_objects.end())
	{
		delete it->second; // Deallocate GameObject.
		it->second = nullptr; // Wrangle dangle.
		m_objects.erase(it);
		m_objects.shrink_to_fit();
	}
}

// Begin TitleState.
TitleState::TitleState() = default;

void TitleState::Enter()
{
	cout << "Entering TitleState..." << endl;
	TEMA::Load("../Assets/img/play.png", "play");
	AddChild("play", new PlayButton({ 0, 0, 400, 100 }, { 412.0f, 384.0f, 200.0f, 50.0f }, "play"));
	SOMA::AllocateChannels(16);
	SOMA::SetMusicVolume(32);
}

void TitleState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_N))
	{
		STMA::ChangeState(new GameState());
		return;
	}
	State::Update();
}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(REMA::GetRenderer(), 255, 0, 0, 255);
	SDL_RenderClear(REMA::GetRenderer());
	State::Render();
}

void TitleState::Exit()
{
	cout << "Exiting TitleState..." << endl;
	TEMA::Unload("play");
	State::Exit();
}
// End TitleState.

// Begin PauseState.
PauseState::PauseState() = default;

void PauseState::Enter()
{
	cout << "Entering PauseState..." << endl;
	AddChild("pauseText", new Label("ltype24", 325, 175, "Paused - press R to resume"));
}

void PauseState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_R))
	{
		STMA::PopState();
		return;
	}
	State::Update();
}

void PauseState::Render()
{
	// First render the GameState
	STMA::GetStates().front()->Render();
	// Now render rest of PauseState
	SDL_SetRenderDrawColor(REMA::GetRenderer(), 0, 0, 255, 128);
	SDL_Rect rect = { 256, 128, 512, 512 };
	SDL_RenderFillRect(REMA::GetRenderer(), &rect);
	State::Render();
}

void PauseState::Exit()
{
	cout << "Exiting PauseState..." << endl;
	State::Exit();
}
// End PauseState.

// Begin GameState.
GameState::GameState() : m_hEuclid(true), m_pathCtr(0), m_pathMax(PFREFRESH) {}

void GameState::Enter()
{
	cout << "Entering GameState..." << endl;
	FOMA::Load("../Assets/img/ltype.TTF", "ltype24", 24);
	FOMA::Load("../Assets/img/ltype.TTF", "ltype14", 14);
	FOMA::Load("../Assets/img/ltype.TTF", "tile", 10);
	TEMA::Load("../Assets/img/Tiles.png", "tiles");
	TEMA::Load("../Assets/img/Maga.png", "player");

	AddChild("level", new TiledLevel(24, 32, 32, 32, "../Assets/dat/Tiledata.txt", "../Assets/dat/Level1.txt", "tiles"));
	AddChild("player", new Player({ 0,0,32,32 }, { (float)(16) * 32, (float)(12) * 32.0f, 32.0f, 32.0f }, GetChild<TiledLevel*>("level")));
	AddChild("bling", new Image({ 224,64,32,32 }, { (float)(16) * 32, (float)(4) * 32.0f, 32.0f, 32.0f }, "tiles"));
	AddChild("menu", new Label("ltype14", 35.0f, 7.0f, "MENU: (~) Toggle Debug | Mouse1/WADS: Move Player | Mouse3: Move Target | Space: Toggle Heuristic", { 255,255,255,255 }));

	// Initialize the H costs for every PathNode in the level.
	std::vector<std::vector<Tile*>>& level = GetChild<TiledLevel*>("level")->GetLevel();
	for (int row = 0; row < kRows; row++)
	{ // Update each node with the default heuristic and set the text for debug mode.
		for (int col = 0; col < kCols; col++)
		{
			if (level[row][col]->Node() == nullptr)
				continue;
			level[row][col]->Node()->SetH(PAMA::HEuclid(level[row][col]->Node(), level[(int)(GetChild("bling")->GetDst()->y / 32)][(int)(GetChild("bling")->GetDst()->x / 32)]->Node()));
		}
	}

	SOMA::SetSoundVolume(32);
	SOMA::SetMusicVolume(16);
}

void GameState::Update()
{
	bool updateLabels = false;
	std::vector<std::vector<Tile*>>& level = GetChild<TiledLevel*>("level")->GetLevel(); // Saving us some space.

	if (EVMA::KeyPressed(SDL_SCANCODE_X))
	{
		STMA::ChangeState(new TitleState()); // Change to new TitleState
		return;
	}
	else if (EVMA::KeyPressed(SDL_SCANCODE_P))
	{
		SOMA::PauseMusic();
		STMA::PushState(new PauseState()); // Add new PauseState
		return;
	}
	else if (EVMA::KeyPressed(SDL_SCANCODE_GRAVE)) // ~ or ` key. Toggle debug mode.
	{
		GetChild<TiledLevel*>("level")->ToggleShowCosts();
		updateLabels = true; // First time debug shown.
	}
	else if (EVMA::KeyPressed(SDL_SCANCODE_SPACE)) // Toggle the heuristic used for pathfinding.
	{
		m_hEuclid = !m_hEuclid;
		updateLabels = true;
		std::cout << "Setting " << (m_hEuclid ? "Euclidian" : "Manhattan") << " heuristic." << std::endl;
	}
	else if (EVMA::MousePressed(1) || EVMA::MousePressed(3)) // If user has clicked.
	{
		int xIdx = (EVMA::GetMousePos().x / 32);
		int yIdx = (EVMA::GetMousePos().y / 32);
		
		if (level[yIdx][xIdx]->IsObstacle() ||	level[yIdx][xIdx]->IsHazard()) // Node() == nullptr;
			return; // We clicked on an invalid tile.
		if (EVMA::MousePressed(1)) // Move the player with left-click.
		{
			GetChild("player")->GetDst()->x = (float)(xIdx * 32);
			GetChild("player")->GetDst()->y = (float)(yIdx * 32);
			m_pathCtr = 0;
		}
		else if (EVMA::MousePressed(3)) // Else move the bling with right-click.
		{
			GetChild("bling")->GetDst()->x = (float)(xIdx * 32);
			GetChild("bling")->GetDst()->y = (float)(yIdx * 32);
			updateLabels = true;
			m_pathCtr = 0;
		}
	}
	if (updateLabels)
	{
		for (int row = 0; row < kRows; row++)
		{ // Update each node with the selected heuristic and set the text for debug mode.
			for (int col = 0; col < kCols; col++)
			{
				if (level[row][col]->Node() == nullptr)
					continue;
				if (m_hEuclid)
					level[row][col]->Node()->SetH(PAMA::HEuclid(level[row][col]->Node(), level[(int)(GetChild("bling")->GetDst()->y / 32)][(int)(GetChild("bling")->GetDst()->x / 32)]->Node()));
				else
					level[row][col]->Node()->SetH(PAMA::HManhat(level[row][col]->Node(), level[(int)(GetChild("bling")->GetDst()->y / 32)][(int)(GetChild("bling")->GetDst()->x / 32)]->Node()));
				level[row][col]->m_lCost->SetText(to_string((int)(level[row][col]->Node()->H())).c_str());
			}
		}
	}
	// Now we can calculate the path. Note: I am not returning a path, only generating one to be rendered.
	if (m_pathCtr++ % m_pathMax == 0) // Pathfinding doesn't have to be every frame.
	{
		PAMA::GetShortestPath(level[(int)(GetChild("player")->GetCenter().y / 32)][(int)(GetChild("player")->GetCenter().x / 32)]->Node(),
			level[(int)(GetChild("bling")->GetCenter().y / 32)][(int)(GetChild("bling")->GetCenter().x / 32)]->Node());
	}
	// Update objects.
	State::Update();
}

void GameState::Render()
{
	SDL_SetRenderDrawColor(REMA::GetRenderer(), 0, 255, 0, 255);
	SDL_RenderClear(REMA::GetRenderer());
	State::Render();
}

void GameState::Exit()
{
	cout << "Exiting GameState..." << endl;
	PAMA::ClearPath();
	TEMA::Unload("tiles");
	TEMA::Unload("player");
	FOMA::Quit();
	State::Exit();
}

void GameState::Resume()
{
	cout << "Resuming GameState..." << endl;
	FOMA::Unload("ltype24");
	SOMA::ResumeMusic();
}
// End GameState