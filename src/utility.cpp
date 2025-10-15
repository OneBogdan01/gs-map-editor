#include "utility.h"
#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
using namespace godot;
PackedStringArray gsg::get_txt_files_in_folder(const String &folder_path)
{
	PackedStringArray txt_files;

	Ref<DirAccess> dir = DirAccess::open(folder_path);
	if (dir.is_null())
	{
		UtilityFunctions::print("Failed to open directory: ", folder_path);
		return txt_files;
	}

	dir->list_dir_begin();
	String file_name = dir->get_next();

	while (file_name.is_empty() == false)
	{
		if (dir->current_is_dir() == false && file_name.get_extension().to_lower() == "txt")
		{
			txt_files.push_back(file_name);
		}
		file_name = dir->get_next();
	}

	dir->list_dir_end();
	return txt_files;
}
String gsg::get_terrain_owner(const String &filename)
{
	String lower_name = filename.to_lower();

	// ============= WATER PROVINCES - Assign to Ocean =============
	// Check water provinces first to avoid conflicts with other checks

	// Major Oceans
	if (lower_name.contains("pacific") || lower_name.contains("atlantic") ||
			lower_name.contains("indian ocean") || lower_name.contains("arctic ocean") ||
			lower_name.contains("southern ocean"))
	{
		return "Ocean";
	}

	// Pacific Ocean regions
	if (lower_name.contains("north pacific") || lower_name.contains("south pacific") ||
			lower_name.contains("east pacific") || lower_name.contains("west pacific") ||
			lower_name.contains("central pacific") || lower_name.contains("northwest pacific") ||
			lower_name.contains("northeast pacific") || lower_name.contains("southwest pacific") ||
			lower_name.contains("southeast pacific"))
	{
		return "Ocean";
	}

	// Atlantic Ocean regions
	if (lower_name.contains("north atlantic") || lower_name.contains("south atlantic") || lower_name.contains("tyne") || lower_name.contains("west atlantic") || lower_name.contains("east atlantic") ||
			lower_name.contains("central atlantic") || lower_name.contains("western atlantic") ||
			lower_name.contains("eastern atlantic"))
	{
		return "Ocean";
	}

	// Indian Ocean regions
	if (lower_name.contains("western indian ocean") || lower_name.contains("eastern indian ocean") ||
			lower_name.contains("central indian ocean") || lower_name.contains("northern indian ocean"))
	{
		return "Ocean";
	}

	// Mediterranean and connected seas
	if (lower_name.contains("mediterranean") || lower_name.contains("western mediterranean") || lower_name.contains("cape bon") || lower_name.contains("cape hatteras") ||
			lower_name.contains("cape howe") || lower_name.contains("cape leuwiin") ||
			lower_name.contains("cape farwell") || lower_name.contains("northwest cape") ||
			lower_name.contains("cape of good hope") || lower_name.contains("cape of storms") ||
			lower_name.contains("firth") || lower_name.contains("approaches") ||
			lower_name.contains("eastern mediterranean") || lower_name.contains("central mediterranean") ||
			lower_name.contains("adriatic") || lower_name.contains("aegean") ||
			lower_name.contains("ionian") || lower_name.contains("tyrrhenian") ||
			lower_name.contains("ligurian") || lower_name.contains("balearic"))
	{
		return "Ocean";
	}

	// European Seas and coastal areas
	if (lower_name.contains("north sea") || lower_name.contains("rockall") ||
			lower_name.contains("helgoland bight") || lower_name.contains("oresund") ||
			lower_name.contains("skagerrack") || lower_name.contains("western approaches") ||
			lower_name.contains("cote") || lower_name.contains("bank") || lower_name.contains("baltic") ||
			lower_name.contains("norwegian sea") || lower_name.contains("barents sea") ||
			lower_name.contains("white sea") || lower_name.contains("kara sea") ||
			lower_name.contains("english channel") || lower_name.contains("irish sea") ||
			lower_name.contains("bay of biscay") || lower_name.contains("gulf of bothnia") ||
			lower_name.contains("kattegat"))
	{
		return "Ocean";
	}

	// Black Sea and connected
	if (lower_name.contains("black sea") || lower_name.contains("sea of azov") ||
			lower_name.contains("marmara"))
	{
		return "Ocean";
	}

	// Red Sea and connected
	if (lower_name.contains("red sea") || lower_name.contains("bal el mandeb") ||
			lower_name.contains("dahlek archipelago") || lower_name.contains("horn of africa") ||
			lower_name.contains("gulf of aden") || lower_name.contains("gulf of suez") ||
			lower_name.contains("gulf of aqaba"))
	{
		return "Ocean";
	}

	// Persian Gulf and Arabian Sea
	if (lower_name.contains("persian gulf") || lower_name.contains("arabian gulf") ||
			lower_name.contains("gulf of oman") || lower_name.contains("arabian sea"))
	{
		return "Ocean";
	}

	// Asian Seas and islands
	if (lower_name.contains("sea of japan") || lower_name.contains("japan basin") ||
			lower_name.contains("east china sea") || lower_name.contains("south china sea") ||
			lower_name.contains("yellow sea") || lower_name.contains("gulf of thailand") ||
			lower_name.contains("andaman sea") || lower_name.contains("bay of bengal") ||
			lower_name.contains("java sea") || lower_name.contains("celebes sea") ||
			lower_name.contains("sulu sea") || lower_name.contains("banda sea") ||
			lower_name.contains("arafura sea") || lower_name.contains("timor sea") ||
			lower_name.contains("gulf of tonkin") || lower_name.contains("sea of okhotsk") ||
			lower_name.contains("bering sea") || lower_name.contains("laquedive islands") ||
			lower_name.contains("comorin cape") || lower_name.contains("ganges delta") ||
			lower_name.contains("irrawady delta") || lower_name.contains("mergui archipelago") ||
			lower_name.contains("nicobar isles") || lower_name.contains("mekong delta") ||
			lower_name.contains("yang tse delta") || lower_name.contains("amur delta") ||
			lower_name.contains("ryukyu islands") || lower_name.contains("paracel islands") ||
			lower_name.contains("philippine trench"))
	{
		return "Ocean";
	}

	// Oceania and Pacific Islands
	if (lower_name.contains("coral sea") || lower_name.contains("tasman sea") ||
			lower_name.contains("bismarck sea") || lower_name.contains("solomon sea") ||
			lower_name.contains("philippine sea") || lower_name.contains("great barrier reef") ||
			lower_name.contains("kangaroo island") || lower_name.contains("great australian bight") ||
			lower_name.contains("king sound") || lower_name.contains("cantebury bight") ||
			lower_name.contains("trobrian islands") || lower_name.contains("new georgia islands") ||
			lower_name.contains("cuyu archipelago"))
	{
		return "Ocean";
	}

	// Caribbean and Gulf of Mexico
	if (lower_name.contains("caribbean") || lower_name.contains("carribean") ||
			lower_name.contains("gulf of mexico") || lower_name.contains("yucatan") ||
			lower_name.contains("antilles") || lower_name.contains("windward islands"))
	{
		return "Ocean";
	}

	// American coast waters
	if (lower_name.contains("gulf of california") || lower_name.contains("hudson bay") ||
			lower_name.contains("gulf of st lawrence") || lower_name.contains("labrador sea") ||
			lower_name.contains("beaufort sea") || lower_name.contains("chukchi sea") ||
			lower_name.contains("baffin bay") || lower_name.contains("davis strait") ||
			lower_name.contains("foxe bassin") || lower_name.contains("rio de janeiro") ||
			lower_name.contains("baia parangua") || lower_name.contains("rio de la plata") ||
			lower_name.contains("bahia blanca") || lower_name.contains("santa catalina islands"))
	{
		return "Ocean";
	}

	// African coast waters
	if (lower_name.contains("mozambique channel") || lower_name.contains("guinea basin") ||
			lower_name.contains("benguela current") || lower_name.contains("cape basin") ||
			lower_name.contains("seychelles") || lower_name.contains("mascareignes") ||
			lower_name.contains("comoros"))
	{
		return "Ocean";
	}

	// Atlantic specific zones
	if (lower_name.contains("gibbs fracture") || lower_name.contains("iberian shelf") ||
			lower_name.contains("blake plataeu") || lower_name.contains("guiana basin") ||
			lower_name.contains("canary approach") || lower_name.contains("cap verde approach") ||
			lower_name.contains("romanche gap") || lower_name.contains("east of magellans") ||
			lower_name.contains("south georgia"))
	{
		return "Ocean";
	}

	// Straits (always water)
	if (lower_name.contains("strait") || lower_name.contains("channel"))
	{
		// Check if it's not an inland strait name used for land provinces
		if (!lower_name.contains("strait settlement") && !lower_name.contains("straits settlement"))
		{
			return "Ocean";
		}
	}

	// Specific named sea zones that might not match patterns above
	if (lower_name.contains("coast"))
	{
		return "Ocean";
	}

	// Generic water identifiers (catch-all for any missed sea zones)
	if ((lower_name.contains("sea") || lower_name.contains("ocean") ||
				lower_name.contains("gulf") || lower_name.contains("bay") ||
				lower_name.contains("bight") || lower_name.contains("basin") ||
				lower_name.contains("delta") || lower_name.contains("reef") ||
				lower_name.contains("sound") || lower_name.contains("archipelago") ||
				lower_name.contains("shelf") || lower_name.contains("trench")) &&
			!lower_name.contains("sea of galilee") && // Inland lake
			!lower_name.contains("dead sea") &&		  // Inland lake
			!lower_name.contains("caspian") &&		  // Inland lake
			!lower_name.contains("aral") &&			  // Inland lake
			!lower_name.contains("great basin"))	  // Land desert
	{
		// Additional safety check - these are definitely land provinces
		if (!lower_name.contains("bayezid") && !lower_name.contains("bayburt") &&
				!lower_name.contains("seas") && // Overseas, etc.
				!lower_name.contains("sea route") && !lower_name.contains("sea trade"))
		{
			return "Ocean";
		}
	}

	return "";
}
