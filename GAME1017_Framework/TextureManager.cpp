#include "TextureManager.h"
#include "RenderManager.h"
#include <iostream>

void TextureManager::Init()
{
	if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != 0)
	{
		std::cout << "Image init success!" << std::endl;
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
		SDL_SetRenderDrawBlendMode(REMA::GetRenderer(), SDL_BLENDMODE_BLEND);
	}
	else
		std::cout << "Image init failed: ERROR - " << IMG_GetError() << std::endl;
}

void TextureManager::Load(const char* path, const std::string key)
{
	SDL_Texture* temp = IMG_LoadTexture(REMA::GetRenderer(), path);
	if (temp == nullptr)
		std::cout << "Could not load texture: Error - " << IMG_GetError() << std::endl;
	else
		s_textures.emplace(key, temp);
}

void TextureManager::Unload(const std::string key)
{
	if (s_textures.find(key) != s_textures.end())
	{
		SDL_DestroyTexture(s_textures[key]);
		s_textures.erase(key);
	}
	else
		std::cout << "Could not unload '" << key << "' - id not found!" << std::endl;
}

SDL_Texture* TextureManager::GetTexture(const std::string key) { return s_textures[key]; }

void TextureManager::Quit()
{
	for (auto const& i : s_textures)
	{
		SDL_DestroyTexture(s_textures[i.first]);
	}
	s_textures.clear();
	IMG_Quit();
}

std::map<std::string, SDL_Texture*> TextureManager::s_textures;