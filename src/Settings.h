#pragma once

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}

	void Load()
	{
				
		constexpr auto path = L"Data/SKSE/Plugins/po3_EssentialFavorites.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);

		detail::get_value(ini, noBarter, "Settings", "No Bartering", ";Prevents favorited items from being sold");
		detail::get_value(ini, noDisarmed, "Settings", "No Disarm", ";Prevents favorited weapons from being disarmed");
		detail::get_value(ini, noAlchemy, "Settings", "No Alchemy", ";Prevents favorited ingredients from being used in alchemy");
		detail::get_value(ini, noCrafting, "Settings", "No Crafting", ";Prevents favorited items from being crafted");
		detail::get_value(ini, noDisenchanting, "Settings", "No Disenchanting", ";Prevents favorited items from being disenchanted");
		detail::get_value(ini, noDrop, "Settings", "No Drop Item", ";Prevents favorited items from being dropped");
		detail::get_value(ini, noGifting, "Settings", "No Gifting", ";Prevents favorited items from being gifted");

		ini.SaveFile(path);
	}

	bool noBarter{ true };
	bool noDisarmed{ true };
	bool noAlchemy{ true };
	bool noCrafting{ true };
	bool noDisenchanting{ true };
	bool noDrop{ false };
	bool noGifting{ false };

private:
	struct detail
	{
		static void get_value(CSimpleIniA& a_ini, bool& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
			a_ini.SetBoolValue(a_section, a_key, a_value, a_comment);
		};
	};
};
