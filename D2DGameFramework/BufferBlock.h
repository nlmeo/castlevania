#pragma once

#include "GameUI/UICanvas.h"
#include "GameUI/UIImage.h"
#include <vector>
class BufferBlock
{
	std::vector<GameUI::UIImage*> mAvailableBufferBar; 
	std::vector<GameUI::UIImage*> mActiveBufferBar;


	bool mIsInit = false;
public:
	BufferBlock() = default;
	~BufferBlock() = default;
	BufferBlock(const BufferBlock&) = delete;
	BufferBlock& operator=(const BufferBlock&) = delete;
	void Init(GameUI::UICanvas* p);
	void Update(float deltaTime);
	void Clear();
	bool IsInit() { return mIsInit; }

	void Spawn(float posX, float posY, float width,float height);

};
