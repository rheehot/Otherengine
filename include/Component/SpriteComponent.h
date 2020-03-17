#pragma once

#include <memory>
#include "ActorComponent.h"
#include "Vector.h"

struct SDL_Renderer;
struct SDL_Texture;

namespace Game
{
	class CSpriteComponent : public CActorComponent
	{
	public:
		explicit CSpriteComponent(AActor& owner, int drawOrder = 100, int updateOrder = 100);
		~CSpriteComponent();

		void BeginPlay() override;
		virtual void Draw() const;
		
		void SetTexture(std::shared_ptr<SDL_Texture>&& texture);
		void SetTexture(const std::shared_ptr<SDL_Texture>& texture);
		void SetTexture(const char* filename);
		[[nodiscard]] SDL_Texture& GetTexture() const { return *texture_; }

		[[nodiscard]] const FVector2<unsigned short>& GetTexSize() const noexcept { return texSize_; }
		[[nodiscard]] int GetDrawOrder() const noexcept { return drawOrder_; }

	private:
		FVector2<uint16_t> texSize_;
		int drawOrder_;
		std::shared_ptr<SDL_Texture> texture_;
	};
}