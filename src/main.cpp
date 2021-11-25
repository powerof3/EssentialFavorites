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
			REL::Relocation<std::uintptr_t> func{ REL::ID(51358) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x106);
		}
	}
	
	namespace Barter
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(51001) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x2B);
		}
	}

	namespace Crafting
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(16049) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x07);
		}
	}

	namespace Disarm
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(37523) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x70);
		}
	}

	namespace Dropped
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(51857) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0x34);
		}
	}

	namespace Enchanting
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(51359) };
			stl::write_thunk_call<detail::Enchanting::IsQuestObject>(func.address() + 0x140);
		}
	}

	namespace Gifting
	{
		void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(51584) };
			stl::write_thunk_call<detail::IsQuestObject>(func.address() + 0xBF);
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

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Version::MAJOR);
	v.PluginName("Essential Favorites");
	v.AuthorName("powerofthree");
	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

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
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	InitializeLog();

	logger::info("loaded");

	SKSE::Init(a_skse);
	
	Settings::GetSingleton()->Load();
	
	SKSE::AllocTrampoline(28);
	
	EssentialFavorites::Install();

	return true;
}
