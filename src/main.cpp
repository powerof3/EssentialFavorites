#include "Settings.h"

namespace EssentialFavorites
{
	namespace detail
	{
		struct IsQuestObject
		{
			static bool thunk(RE::InventoryEntryData* a_data)
			{
				auto result = func(a_data);
				if (!result && a_data) {
					result = a_data->IsFavorited();
				}
				return result;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		namespace Enchanting
		{
			struct IsQuestObject
			{
				static bool thunk(RE::InventoryEntryData* a_data)
				{
					auto result = func(a_data);
					if (!result && a_data) {
						result = a_data->IsEnchanted() && a_data->IsFavorited();
					}
					return result;
				}
				static inline REL::Relocation<decltype(thunk)> func;
			};
		}
	}

	namespace Alchemy
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(50453) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x106);

			logger::info("patched alchemy");
		}
	}
	
	namespace Barter
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(50061) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x25);

			logger::info("patched bartering");
		}
	}

	namespace Crafting
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(15811) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x07);

			logger::info("patched crafting");
		}
	}

	namespace Disarm
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(36523) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x71);

			logger::info("patched disarming");
		}
	}

	namespace Dropped
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(50978) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x38);

			logger::info("patched dropping");
		}
	}

	namespace Enchanting
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(50454) };
			stl::write_thunk_call<detail::Enchanting::IsQuestObject>(func.address() + 0x133);

			logger::info("patched enchanting");
		}
	}

	namespace Gifting
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(50688) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x1C);

			logger::info("patched gifting");
		}
	}

	void Install()
	{
		const auto settings = Settings::GetSingleton();
		if (settings->noAlchemy) {
			Alchemy::Patch();
		}
		if (settings->noBarter) {
			Barter::Patch();
		}
		if (settings->noCrafting) {
			Crafting::Patch();
		}
		if (settings->noDisarmed) {
			Disarm::Patch();
		}
		if (settings->noDisenchanting) {
			Enchanting::Patch();
		}
		if (settings->noDrop) {
			Dropped::Patch();
		}
		if (settings->noGifting) {
			Gifting::Patch();
		}
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "Favorite Misc Items";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("loaded plugin");

	SKSE::Init(a_skse);

	Settings::GetSingleton()->Load();
	SKSE::AllocTrampoline(28);
	
	EssentialFavorites::Install();

	return true;
}
