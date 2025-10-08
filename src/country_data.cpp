#include "country_data.h"
#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/file_access.hpp"

#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/print_string.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <cassert>
#include <cstdint>

using namespace godot;
static String get_terrain_owner(const String &filename)
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
			!lower_name.contains("dead sea") && // Inland lake
			!lower_name.contains("caspian") && // Inland lake
			!lower_name.contains("aral") && // Inland lake
			!lower_name.contains("great basin")) // Land desert
	{
		// Additional safety check - these are definitely land provinces
		if (!lower_name.contains("bayezid") && !lower_name.contains("bayburt") &&
				!lower_name.contains("seas") && // Overseas, etc.
				!lower_name.contains("sea route") && !lower_name.contains("sea trade"))
		{
			return "Ocean";
		}
	}

	// ============= MOUNTAINS =============

	// Alps - distributed among Alpine nations
	if (lower_name.contains("alps"))
	{
		if (lower_name.contains("1"))
		{
			return "MLO"; // Western Alps to Milan
		}
		if (lower_name.contains("2"))
		{
			return "HAB"; // Central Alps to Austria
		}
		if (lower_name.contains("3"))
		{
			return "HAB"; // Tyrolean Alps to Austria (Habsburg)
		}
		if (lower_name.contains("4"))
		{
			return "SAV"; // Southern Alps to Savoy
		}
		return "SWI"; // Default to Switzerland
	}

	// Carpathian Mountains
	if (lower_name.contains("carpathian"))
	{
		if (lower_name.contains("west"))
		{
			return "HUN"; // Western Carpathians to Hungary
		}
		if (lower_name.contains("east") || lower_name.contains("transylvan"))
		{
			return "WAL"; // Eastern/Transylvanian Carpathians to Wallachia
		}
		return "HUN";
	}

	// Pyrenees Mountains
	if (lower_name.contains("pyrenees"))
	{
		if (lower_name.contains("west"))
		{
			return "CAS"; // Western Pyrenees to Castile
		}
		if (lower_name.contains("east") || lower_name.contains("aragon"))
		{
			return "ARA"; // Eastern Pyrenees to Aragon
		}
		if (lower_name.contains("navarre"))
		{
			return "NAV"; // Navarre section
		}
		return "CAS";
	}

	// Caucasus and nearby mountains
	if (lower_name.contains("caucasus") || lower_name.contains("caucasia"))
	{
		if (lower_name.contains("north"))
		{
			return "CIR"; // North Caucasus to Circassia
		}
		if (lower_name.contains("south") || lower_name.contains("central"))
		{
			return "GEO"; // South/Central to Georgia
		}
		return "GEO";
	}

	if (lower_name.contains("elburz") || lower_name.contains("alborz"))
	{
		return "QOM"; // Qara Qoyunlu (1444)
	}

	if (lower_name.contains("armenian highlands"))
	{
		return "GEO"; // Georgia/Armenia area
	}

	// Ural Mountains
	if (lower_name.contains("ural"))
	{
		if (lower_name.contains("north"))
		{
			return "MUS"; // Northern Urals to Muscovy
		}
		if (lower_name.contains("south"))
		{
			return "NOG"; // Southern Urals to Nogai/Great Horde area
		}
		return "MUS"; // Default to Muscovy
	}

	// Himalaya and Central Asian Mountains
	if (lower_name.contains("himalaya"))
	{
		if (lower_name.contains("west") || lower_name.contains("kashmir"))
		{
			return "KAS"; // Western Himalayas to Kashmir
		}
		if (lower_name.contains("central") || lower_name.contains("nepal"))
		{
			return "NEP"; // Central Himalayas to Nepal
		}
		if (lower_name.contains("east") || lower_name.contains("bhutan"))
		{
			return "BHU"; // Eastern Himalayas to Bhutan
		}
		if (lower_name.contains("tibet"))
		{
			return "TIB"; // Tibetan Himalayas to Tibet (U-Tsang)
		}
		return "TIB"; // Default to Tibet
	}

	if (lower_name.contains("karakoram"))
	{
		return "KAS"; // Kashmir area
	}

	if (lower_name.contains("kunlun"))
	{
		return "TIB"; // Tibet
	}

	if (lower_name.contains("tiam shan") || lower_name.contains("tian shan"))
	{
		return "YAR"; // Yarkand/Chagatai area
	}

	if (lower_name.contains("changbai"))
	{
		return "MJZ"; // Manchu (Jianzhou)
	}

	// Southeast Asian Mountains
	if (lower_name.contains("annamite"))
	{
		return "DAI"; // Dai Viet
	}

	if (lower_name.contains("arakan mountains"))
	{
		return "MKA"; // Mrauk-U (Arakan)
	}

	if (lower_name.contains("dawna hills"))
	{
		return "AVA"; // Ava
	}

	if (lower_name.contains("gaoligong mountains"))
	{
		return "MNG"; // Ming China
	}

	if (lower_name.contains("shan highlands"))
	{
		return "AVA"; // Ava
	}

	if (lower_name.contains("naga hills"))
	{
		return ""; // Tribal/uncolonized
	}

	// Andes Mountains
	if (lower_name.contains("andes"))
	{
		if (lower_name.contains("north"))
		{
			return "QUI"; // Northern Andes to Quito
		}
		if (lower_name.contains("central"))
		{
			return "INC"; // Central Andes to Inca
		}
		if (lower_name.contains("south"))
		{
			return "MPU"; // Southern Andes to Mapuche
		}
		return "INC";
	}

	// North American Mountains
	if (lower_name.contains("rockies") || lower_name.contains("rocky"))
	{
		if (lower_name.contains("north"))
		{
			return "BLA"; // Northern Rockies to Blackfoot
		}
		if (lower_name.contains("south"))
		{
			return "SHO"; // Southern Rockies to Shoshone
		}
		return "SHO"; // Default to Shoshone
	}

	if (lower_name.contains("appalachian") || lower_name.contains("allegheny") ||
			lower_name.contains("cumberland mountains") || lower_name.contains("great smoky") ||
			lower_name.contains("blue ridge") || lower_name.contains("blue mountains") ||
			lower_name.contains("white mountains") || lower_name.contains("adirondacks") ||
			lower_name.contains("laurentian massif"))
	{
		return "CHE"; // Cherokee territory (eastern mountains)
	}

	// Atlas Mountains
	if (lower_name.contains("atlas") || lower_name.contains("aures"))
	{
		return "MOR"; // Morocco
	}

	// Anatolian Mountains (Taurus, Pontic)
	if (lower_name.contains("taurus") || lower_name.contains("pontic"))
	{
		return "OTT"; // Ottoman Empire
	}

	// Zagros and Suleiman Range
	if (lower_name.contains("zagros"))
	{
		return "QOM"; // Qara Qoyunlu (1444)
	}

	if (lower_name.contains("suleiman range"))
	{
		return ""; // Tribal Afghanistan area
	}

	// Scandinavian Mountains
	if (lower_name.contains("jotenheimen") || lower_name.contains("skands"))
	{
		return "NOR"; // Norway
	}

	// ============= DESERTS =============

	// Sahara Desert - divided regionally
	if (lower_name.contains("sahara"))
	{
		if (lower_name.contains("west") || lower_name.contains("mauritania"))
		{
			return "MOR"; // Western Sahara to Morocco
		}
		if (lower_name.contains("central") || lower_name.contains("libya"))
		{
			return "TRP"; // Central Sahara to Tripoli
		}
		if (lower_name.contains("east") || lower_name.contains("egypt"))
		{
			return "MAM"; // Eastern Sahara to Mamluks
		}
		if (lower_name.contains("south") || lower_name.contains("mali"))
		{
			return "MAL"; // Southern Sahara to Mali
		}
		return "MOR";
	}

	// North African deserts
	if (lower_name.contains("el djouf") || lower_name.contains("chott el-jerid") ||
			lower_name.contains("tajhari") || lower_name.contains("djado"))
	{
		return "MOR"; // Morocco/Maghreb region
	}

	if (lower_name.contains("egyptian desert"))
	{
		return "MAM"; // Mamluks
	}

	if (lower_name.contains("badiyat ash sham"))
	{
		return "MAM"; // Mamluks (Syrian desert)
	}

	// Arabian Desert (Rub' al Khali)
	if (lower_name.contains("arabian") || lower_name.contains("rub") ||
			lower_name.contains("al khali") || lower_name.contains("tubaiq"))
	{
		if (lower_name.contains("north"))
		{
			return "NAJ"; // Najd
		}
		if (lower_name.contains("south") || lower_name.contains("yemen"))
		{
			return "YEM"; // Yemen
		}
		return "NAJ";
	}

	// Central Asian Deserts
	if (lower_name.contains("gobi"))
	{
		if (lower_name.contains("west"))
		{
			return "MNG"; // Mongolia (Mongol remnants)
		}
		if (lower_name.contains("east") || lower_name.contains("inner"))
		{
			return "MNG"; // Still Mongolia sphere in 1444
		}
		return "MNG";
	}

	if (lower_name.contains("taklamakan") || lower_name.contains("takla makan") ||
			lower_name.contains("tarim"))
	{
		return "YAR"; // Yarkand
	}

	if (lower_name.contains("chagai"))
	{
		return ""; // Tribal/uncolonized Baluchistan
	}

	// African Deserts
	if (lower_name.contains("kalahari"))
	{
		return "BUT"; // Butua
	}

	if (lower_name.contains("namib"))
	{
		return "KON"; // Kongo
	}

	// Australian Deserts
	if (lower_name.contains("great sandy") || lower_name.contains("gibson") ||
			lower_name.contains("great victoria") || lower_name.contains("simpson") ||
			lower_name.contains("outback"))
	{
		return ""; // Uncolonized at game start
	}

	// North American Deserts
	if (lower_name.contains("sonoran") || lower_name.contains("mojave") ||
			lower_name.contains("great basin"))
	{
		return ""; // Uncolonized tribal areas
	}

	// South American Deserts
	if (lower_name.contains("atacama"))
	{
		return ""; // Uncolonized in 1444
	}

	if (lower_name.contains("patagonia") && lower_name.contains("desert"))
	{
		return ""; // Uncolonized/tribal in 1444
	}

	// ============= TUNDRA/TAIGA/ICE =============

	// Siberian Wasteland
	if (lower_name.contains("siberia"))
	{
		if (lower_name.contains("west"))
		{
			return "SIB"; // Sibir Khanate
		}
		if (lower_name.contains("east"))
		{
			return ""; // Mostly uncolonized tribal areas
		}
		return "SIB"; // Default to Sibir Khanate
	}

	if (lower_name.contains("yamal") || lower_name.contains("naryan-mar"))
	{
		return "NOV";
	}

	// Arctic/Greenland
	if (lower_name.contains("greendland tip"))
	{
		return "Ocean";
	}
	if (lower_name.contains("arctic") || lower_name.contains("greenland") ||
			lower_name.contains("nunavut") || lower_name.contains("northwest terretories") ||
			lower_name.contains("baffin"))
	{
		if (lower_name.contains("canada") || lower_name.contains("north america"))
		{
			return ""; // Uninhabited
		}
		if (lower_name.contains("greenland"))
		{
			return "DAN"; // Norway (under Danish crown)
		}
		return ""; // Most Arctic is uninhabited
	}

	// Iceland interior (if considered wasteland parts)
	if (lower_name.contains("iceland") && lower_name.contains("interior"))
	{
		return "ICE"; // Iceland
	}

	// Alaska and northern territories
	if (lower_name.contains("alaska") || lower_name.contains("columbia"))
	{
		return ""; // Uncolonized tribal lands
	}

	// ============= JUNGLES/RAINFORESTS =============

	// Amazon Rainforest
	if (lower_name.contains("amazon") || lower_name.contains("ucayali") ||
			lower_name.contains("guyana"))
	{
		return ""; // Uncolonized tribes
	}

	// Congo Jungle
	if (lower_name.contains("congo"))
	{
		return "KON"; // Kongo
	}

	// Papua/New Guinea
	if (lower_name.contains("papua") || lower_name.contains("manokwari") ||
			lower_name.contains("asmat") || lower_name.contains("yos sudarso") ||
			lower_name.contains("daru") || lower_name.contains("kerema") ||
			lower_name.contains("lae") || lower_name.contains("madang") ||
			lower_name.contains("fak-fak") || lower_name.contains("rabaul") ||
			lower_name.contains("hanuabada"))
	{
		return ""; // Uncolonized tribal areas
	}

	// Borneo interior
	if (lower_name.contains("borneo") && !lower_name.contains("north"))
	{
		return ""; // Uncolonized interior
	}

	if (lower_name.contains("north borneo"))
	{
		return "BRU"; // Brunei
	}

	// Sumatra interior
	if (lower_name.contains("sumatra") || lower_name.contains("minang") ||
			lower_name.contains("sampit") || lower_name.contains("katapang") ||
			lower_name.contains("tagloc"))
	{
		return ""; // Uncolonized tribal areas
	}

	// Other Southeast Asian islands
	if (lower_name.contains("mentawai") || lower_name.contains("nias") ||
			lower_name.contains("yamdena") || lower_name.contains("aru") ||
			lower_name.contains("moluccas"))
	{
		return ""; // Uncolonized tribal areas
	}

	// ============= INLAND WATERS/LAKES =============
	// These should be assigned to countries

	// Caspian Sea (actually a lake, despite the name)
	if (lower_name.contains("caspian"))
	{
		if (lower_name.contains("north"))
		{
			return "GOL"; // Golden Horde remnants/Astrakhan area
		}
		if (lower_name.contains("west"))
		{
			return "SRV"; // Shirvan
		}
		if (lower_name.contains("south"))
		{
			return "QOM"; // Qara Qoyunlu (1444)
		}
		if (lower_name.contains("east"))
		{
			return "KHO"; // Khoresm area
		}
		return "QOM"; // Default to Qara Qoyunlu
	}

	// Central Asian Lakes
	if (lower_name.contains("aral"))
	{
		return "KHO"; // Khoresm area in 1444
	}

	if (lower_name.contains("balkhash") || lower_name.contains("balchasj"))
	{
		return "OIR"; // Oirat/Mongol sphere
	}

	if (lower_name.contains("zaysan"))
	{
		return "OIR"; // Oirat area
	}

	if (lower_name.contains("alakol"))
	{
		return "KAZ"; // Kazakh area
	}

	if (lower_name.contains("issyk kul"))
	{
		return "KOK"; // Kokand area
	}

	if (lower_name.contains("baikal") || lower_name.contains("bajkal"))
	{
		return ""; // Far eastern Siberia, mostly uncontrolled in 1444
	}

	// European Lakes
	if (lower_name.contains("balaton"))
	{
		return "HUN"; // Hungary
	}

	if (lower_name.contains("geneva"))
	{
		return "SAV"; // Savoy
	}

	if (lower_name.contains("bodensee"))
	{
		return "HAB"; // Austria
	}

	if (lower_name.contains("vistula"))
	{
		return "TEU"; // Teutonic Order
	}

	// Swedish/Finnish Lakes
	if (lower_name.contains("vanern") || lower_name.contains("vattern") ||
			lower_name.contains("malaren") || lower_name.contains("hjalmaren"))
	{
		return "SWE"; // Sweden
	}

	if (lower_name.contains("alands hav"))
	{
		return "Ocean"; // Sweden (Åland)
	}

	if (lower_name.contains("finish lakes") || lower_name.contains("saimaa") ||
			lower_name.contains("oulujarvi"))
	{
		return "SWE"; // Sweden (Finland under Swedish rule in 1444)
	}

	if (lower_name.contains("storavan") || lower_name.contains("storsjon"))
	{
		return "SWE"; // Sweden
	}

	// Russian Lakes
	if (lower_name.contains("ladoga"))
	{
		return "NOV"; // Novgorod
	}

	if (lower_name.contains("onega"))
	{
		return "NOV"; // Novgorod
	}

	if (lower_name.contains("peipus") || lower_name.contains("peipsi jarv"))
	{
		return "LIV"; // Livonian Order
	}

	// Middle Eastern Lakes
	if (lower_name.contains("van") && lower_name.contains("lake"))
	{
		return "AKK"; // Ak Koyunlu
	}

	if (lower_name.contains("tuz gola") || lower_name.contains("lake daryacheh"))
	{
		return "QOM"; // Qara Qoyunlu
	}

	if (lower_name.contains("sevan"))
	{
		return "GEO"; // Georgia/Armenia area
	}

	// Chinese Lakes
	if (lower_name.contains("qinghai") || lower_name.contains("siling") ||
			lower_name.contains("taihu") || lower_name.contains("hongze") ||
			lower_name.contains("dongting") || lower_name.contains("poyang"))
	{
		return "MNG"; // Ming China
	}

	if (lower_name.contains("tonie sap"))
	{
		return "AYU"; // Ayutthaya
	}

	// African Lakes
	if (lower_name.contains("victoria") && lower_name.contains("lake"))
	{
		return "BUG"; // Buganda
	}

	if (lower_name.contains("tanganyika") || lower_name.contains("tangayika"))
	{
		return ""; // Uncolonized tribal areas
	}

	if (lower_name.contains("malawi") || lower_name.contains("nyasa"))
	{
		return ""; // Uncolonized tribal areas
	}

	if (lower_name.contains("chad") && lower_name.contains("lake"))
	{
		return "BOR"; // Bornu
	}

	if (lower_name.contains("tana lake"))
	{
		return "ETH"; // Ethiopia
	}

	if (lower_name.contains("turkana"))
	{
		return ""; // Uncolonized tribal areas
	}

	if (lower_name.contains("kama") || lower_name.contains("mweru") ||
			lower_name.contains("kivu") || lower_name.contains("rutanzige") ||
			lower_name.contains("albert"))
	{
		return ""; // Central African lakes, uncolonized
	}

	// Dead Sea and Sea of Galilee
	if (lower_name.contains("dead sea"))
	{
		return "MAM"; // Mamluks
	}

	if (lower_name.contains("galilee") && lower_name.contains("sea"))
	{
		return "MAM"; // Mamluks
	}

	// South American Lakes
	if (lower_name.contains("titicaca"))
	{
		return "INC"; // Inca
	}

	if (lower_name.contains("poopo"))
	{
		return "INC"; // Inca
	}

	if (lower_name.contains("nicaragua") && lower_name.contains("lake"))
	{
		return ""; // Uncolonized in 1444
	}

	// North American Great Lakes
	if (lower_name.contains("great lakes") ||
			(lower_name.contains("lake") &&
					(lower_name.contains("superior") || lower_name.contains("michigan") ||
							lower_name.contains("huron") || lower_name.contains("erie") ||
							lower_name.contains("ontario"))))
	{
		return "HUR"; // Huron
	}

	// Canadian Lakes
	if (lower_name.contains("winnipeg") || lower_name.contains("nipigon") ||
			lower_name.contains("champlain") || lower_name.contains("lac saint jean") ||
			lower_name.contains("manicouagan") || lower_name.contains("michikamau") ||
			lower_name.contains("bienville") || lower_name.contains("al eau claire") ||
			lower_name.contains("minto") || lower_name.contains("la grande riviere") ||
			lower_name.contains("island") || lower_name.contains("reindeer") ||
			lower_name.contains("wollaston") || lower_name.contains("great slave") ||
			lower_name.contains("athabasca") || lower_name.contains("dubawni") ||
			lower_name.contains("yathkyed") || lower_name.contains("kasba") ||
			lower_name.contains("nueltin") || lower_name.contains("wholdaia") ||
			lower_name.contains("peter pond") || lower_name.contains("williston"))
	{
		return ""; // Uncolonized Canadian territories
	}

	// US Lakes
	if (lower_name.contains("great salt lake") || lower_name.contains("tulare") ||
			lower_name.contains("cahuilla"))
	{
		return ""; // Uncolonized western territories
	}

	if (lower_name.contains("everglades"))
	{
		return ""; // Uncolonized Florida
	}

	// ============= PLATEAUS & HIGHLANDS =============

	// Tibetan Plateau
	if (lower_name.contains("tibet") && (lower_name.contains("plateau") || lower_name.contains("high")))
	{
		return "TIB"; // Tibet (U-Tsang)
	}

	if (lower_name.contains("changtang"))
	{
		return "TIB"; // Tibet
	}

	// Altiplano
	if (lower_name.contains("altiplano"))
	{
		return "INC"; // Inca
	}

	// Iranian Plateau
	if (lower_name.contains("iranian plateau") || lower_name.contains("dasht"))
	{
		return "QOM"; // Qara Qoyunlu in 1444
	}

	// Deccan Plateau
	if (lower_name.contains("deccan"))
	{
		return "BAH"; // Bahmani Sultanate (dominant in 1444)
	}

	// Anatolian Plateau
	if (lower_name.contains("anatolian") && lower_name.contains("plateau"))
	{
		return "OTT"; // Ottoman Empire
	}

	// Indian Highlands
	if (lower_name.contains("meghalaya") || lower_name.contains("arunachal"))
	{
		return ""; // Tribal northeastern India
	}

	// African Highlands
	if (lower_name.contains("highveld") || lower_name.contains("transvaal"))
	{
		return ""; // Uncolonized southern Africa
	}

	// ============= PACIFIC ISLANDS =============
	// Most are uncolonized in 1444

	if (lower_name.contains("solomon islands") || lower_name.contains("vanuatu") ||
			lower_name.contains("kiribati") || lower_name.contains("tahiti") ||
			lower_name.contains("new caledonia") || lower_name.contains("society islands") ||
			lower_name.contains("rapa nui") || lower_name.contains("tuvalu") ||
			lower_name.contains("gilbert islands") || lower_name.contains("nauru") ||
			lower_name.contains("marshall") || lower_name.contains("masrshall") ||
			lower_name.contains("wake") || lower_name.contains("micronesia") ||
			lower_name.contains("midway") || lower_name.contains("christmas island") ||
			lower_name.contains("cocos island") || lower_name.contains("palau"))
	{
		return ""; // Uncolonized Pacific islands
	}

	// ============= TRIBAL/COLONIZABLE LANDS =============
	// These remain unassigned as they're colonizable or tribal

	// Australian Aboriginal lands
	if (lower_name.contains("wergaia") || lower_name.contains("wiradjuri") ||
			lower_name.contains("kamilaroi") || lower_name.contains("bundjalung") ||
			lower_name.contains("barunggam") || lower_name.contains("guugu yimithirr") ||
			lower_name.contains("wulgurukaba") || lower_name.contains("baiali") ||
			lower_name.contains("yuin") || lower_name.contains("gunditjmara") ||
			lower_name.contains("nukunu") || lower_name.contains("kurtjar") ||
			lower_name.contains("biri") || lower_name.contains("gureng") ||
			lower_name.contains("wuliwuli") || lower_name.contains("bidjara") ||
			lower_name.contains("badtjala") || lower_name.contains("yiman") ||
			lower_name.contains("tharawal") || lower_name.contains("gundungurra") ||
			lower_name.contains("dharug") || lower_name.contains("waveroo") ||
			lower_name.contains("ngadjuri") || lower_name.contains("nyaki") ||
			lower_name.contains("yuat") || lower_name.contains("kurrama") ||
			lower_name.contains("mangala") || lower_name.contains("wunambul") ||
			lower_name.contains("australian lakes"))
	{
		return ""; // Uncolonized Aboriginal lands
	}

	// New Zealand
	if (lower_name.contains("timaru") || lower_name.contains("ata whenua"))
	{
		return ""; // Uncolonized Maori lands
	}

	// African tribal lands
	if (lower_name.contains("bonny") || lower_name.contains("calabar") ||
			lower_name.contains("cameroon") || lower_name.contains("gabon") ||
			lower_name.contains("lesser namaqualand") || lower_name.contains("roggeveld") ||
			lower_name.contains("great karoo") || lower_name.contains("transkei") ||
			lower_name.contains("taveta") || lower_name.contains("beafada") ||
			lower_name.contains("karou") || lower_name.contains("bagoe") ||
			lower_name.contains("tswana") || lower_name.contains("swazi") ||
			lower_name.contains("upper limpopo") || lower_name.contains("griqualand") ||
			lower_name.contains("kinga") || lower_name.contains("makonde") ||
			lower_name.contains("phangwa") || lower_name.contains("lala") ||
			lower_name.contains("luangwa") || lower_name.contains("ewaso ngiro") ||
			lower_name.contains("dawa") || lower_name.contains("kuria") ||
			lower_name.contains("shamba") || lower_name.contains("gogo") ||
			lower_name.contains("nyatura") || lower_name.contains("zigua") ||
			lower_name.contains("shaga") || lower_name.contains("kikuyu") ||
			lower_name.contains("hehe") || lower_name.contains("lango") ||
			lower_name.contains("muhambwe") || lower_name.contains("rukwa") ||
			lower_name.contains("bemba") || lower_name.contains("lamba") ||
			lower_name.contains("lenje") || lower_name.contains("kaonde") ||
			lower_name.contains("lega") || lower_name.contains("uganda") ||
			lower_name.contains("malawi") || lower_name.contains("limpopo") ||
			lower_name.contains("tana") || lower_name.contains("rio negro") ||
			lower_name.contains("senqu") || lower_name.contains("sergpipe") ||
			lower_name.contains("jeremoabo") || lower_name.contains("inhambupe") ||
			lower_name.contains("pambu") || lower_name.contains("borborema") ||
			lower_name.contains("teush"))
	{
		return ""; // Uncolonized tribal Africa
	}

	// South American tribal lands
	if (lower_name.contains("florida blanca") || lower_name.contains("tehuelmapu") ||
			lower_name.contains("poya") || lower_name.contains("tierra del fuego"))
	{
		return ""; // Uncolonized Patagonia/southern South America
	}

	// North American Native lands
	if (lower_name.contains("quebec") || lower_name.contains("weagamow") ||
			lower_name.contains("yuki") || lower_name.contains("maidu") ||
			lower_name.contains("shasta") || lower_name.contains("klamath") ||
			lower_name.contains("umatilla") || lower_name.contains("hualapai") ||
			lower_name.contains("hopi") || lower_name.contains("jicarilla") ||
			lower_name.contains("eelalapito") || lower_name.contains("baachinena") ||
			lower_name.contains("utsehta") || lower_name.contains("itscheabine") ||
			lower_name.contains("calusa") || lower_name.contains("mayaimi") ||
			lower_name.contains("naskapi") || lower_name.contains("hebina") ||
			lower_name.contains("nisga'a") || lower_name.contains("heiltsuk") ||
			lower_name.contains("asinaan") || lower_name.contains("teton") ||
			lower_name.contains("nimiipu") || lower_name.contains("taaqtam") ||
			lower_name.contains("wintun") || lower_name.contains("otoe") ||
			lower_name.contains("neozho") || lower_name.contains("panka") ||
			lower_name.contains("selis") || lower_name.contains("hunkpapha") ||
			lower_name.contains("wadopabina") || lower_name.contains("arapaho") ||
			lower_name.contains("kansas") || lower_name.contains("mandan") ||
			lower_name.contains("seminole") || lower_name.contains("nootka") ||
			lower_name.contains("kwakiutl") || lower_name.contains("tsimshian") ||
			lower_name.contains("labrador"))
	{
		return ""; // Uncolonized Native American lands
	}

	// Southeast Asian tribal/uncolonized
	if (lower_name.contains("kelang") || lower_name.contains("middag") ||
			lower_name.contains("ifugao") || lower_name.contains("caraga") ||
			lower_name.contains("mingin"))
	{
		return ""; // Uncolonized Southeast Asia
	}

	// Indonesian islands
	if (lower_name.contains("sumbawa") || lower_name.contains("sumba"))
	{
		return ""; // Uncolonized Indonesian islands
	}

	return ""; // Default: unassigned wasteland
}
CountryData::CountryData() {}
CountryData::~CountryData() {}
Array CountryData::get_province_data() const
{
	return province_data;
}
Array CountryData::get_country_color_data() const
{
	return country_color_data;
}
void CountryData::set_province_data(const Array &data)
{
	province_data = data;
}
void CountryData::set_provinces_folder(const String &p_path)
{
	provinces_folder_path = p_path;
}
String CountryData::get_provinces_folder() const
{
	return provinces_folder_path;
}

void CountryData::set_countries_folder(const String &p_path)
{
	countries_folder_path = p_path;
}

String CountryData::get_countries_folder() const
{
	return countries_folder_path;
}
void CountryData::set_countries_color_folder(const String &p_path)
{
	country_colors_folder_path = p_path;
}

String CountryData::get_countries_color_folder() const
{
	return country_colors_folder_path;
}
PackedStringArray CountryData::get_txt_files_in_folder(const String &folder_path)
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
//call at the beginning
void CountryData::build_look_up_tables()
{
	country_id_to_country_name.clear();
	country_name_to_color.clear();

	for (const Dictionary &dict : country_data)
	{
		country_id_to_country_name[dict["Id"]] = dict["Name"];
	}

	for (const Dictionary &dict : country_color_data)
	{
		country_name_to_color[dict["Name"]] = dict["Color"];
	}
}
Color CountryData::get_country_color_lookup(const String &country_id)
{
	String country_name = country_id_to_country_name[country_id];

	return Color(country_name_to_color[country_name]);
}
PackedInt32Array CountryData::populate_color_map_buffers()
{
	PackedInt32Array data;

	int i = 0;
	for (const Dictionary &dict : province_data)
	{
		int id = dict["Id"];
		String country_id = dict["Owner"];
		Color color = get_country_color_lookup(country_id);

		data.append_array({ color.get_r8(), color.get_g8(), color.get_b8(), id });
	}
	return data;
}
bool CountryData::sort_by_id(const Dictionary &a, const Dictionary &b)
{
	return (int)a["Id"] < (int)b["Id"];
}
void CountryData::parse_all_files()
{
	UtilityFunctions::print("Parsing countries ...");

	if (countries_folder_path.is_empty())
	{
		UtilityFunctions::print("Countries folder path is not set!");
		return;
	}
	province_data.clear();
	country_color_data.clear();
	country_data.clear();
	PackedStringArray country_filenames = get_txt_files_in_folder(countries_folder_path);
	PackedStringArray country_color_filenames = get_txt_files_in_folder(country_colors_folder_path);
	PackedStringArray province_filenames = get_txt_files_in_folder(provinces_folder_path);
	for (const auto &country_color_filename : country_color_filenames)
	{
		// filenames are like this: Aachen.txt
		Dictionary country_colors;
		country_colors["Name"] = country_color_filename.split(".")[0];
		String filename = country_color_filename.get_file();
		String full_path = country_colors_folder_path.path_join(country_color_filename);
		Color country_color = parse_country_color(full_path);
		country_colors["Color"] = country_color;
		country_color_data.push_back(country_colors);
	}

	for (const auto &country_filename : country_filenames)
	{
		Dictionary country_codes;
		// this is how the file names are AAC - Aachen.txt
		country_codes["Id"] = country_filename.substr(0, 3); // gets AAC identifier
		country_codes["Name"] = country_filename.substr(6).split(".")[0]; // get country name Aachen

		country_data.push_back(country_codes);
	}
	// Add terrain colors here:

	terrain_tokens["Ocean"] = Color(0.1, 0.4, 0.7);

	terrain_tokens["No Owner"] = Color(0.7, 0.5, 0.1);

	Array terrain_keys = terrain_tokens.keys();
	for (const auto &terrain_key : terrain_keys)
	{
		Dictionary terrain_dict;
		String terrain_name = terrain_key;
		terrain_dict["Name"] = terrain_name;
		terrain_dict["Color"] = terrain_tokens[terrain_name];
		country_color_data.push_back(terrain_dict);
		Dictionary terrain_data;
		terrain_data["Id"] = terrain_name;
		terrain_data["Name"] = terrain_name;
		country_data.push_back(terrain_data);
	}
	for (const auto &province_filename : province_filenames)
	{
		//1-Uppland.txt
		Dictionary province_codes;
		String filename = province_filename.get_file();

		PackedStringArray parts = filename.split(" - ");
		if (filename.contains(" - "))
		{
			parts = filename.split(" - ");
		}
		else if (filename.contains("-"))
		{
			parts = filename.split("-");
		}
		// Check if we got the expected number of parts
		if (parts.size() < 2)
		{
			continue; // Skip this file
		}
		province_codes["Id"] = parts[0].strip_edges().to_int();
		province_codes["Name"] = parts[1].split(".txt")[0].strip_edges(); // gets "Uppland"
		String full_path = provinces_folder_path.path_join(province_filename);
		province_codes["Owner"] = parse_province_owner(full_path);
		province_data.push_back(province_codes);
	}
	// sort them by id
	province_data.sort_custom(callable_mp(this, &CountryData::sort_by_id));
	// create lookup tables
	build_look_up_tables();
	UtilityFunctions::print("Parsed Provinces:", province_data.size());
	UtilityFunctions::print("Parsed Country Colors:", country_color_data.size());
	UtilityFunctions::print("Parsed Countries:", country_data.size());
}
PackedStringArray CountryData::get_country_provinces(uint32_t country_index)
{
	PackedStringArray provinces;
	Dictionary country_dict = country_data[country_index];
	String country_id = country_dict["Id"];

	for (const auto &j : province_data)
	{
		Dictionary province_dict = j;
		String owner_id = province_dict["Owner"];

		if (owner_id == country_id)
		{
			provinces.push_back("  Province: " + String(province_dict["Name"]) + " (ID: " + itos(province_dict["Id"]) + ")");
		}
	}
	return provinces;
}
String CountryData::get_country_from_province(int32_t province_id)
{
	String country_owner = "None";
	for (const Dictionary &dict : province_data)
	{
		if (int32_t(dict["Id"]) == province_id)
		{
			String country_id = String(dict["Owner"]);
			for (const Dictionary &entry_country : country_data)
			{
				if (String(entry_country["Id"]) == country_id)
				{
					country_owner = (entry_country["Name"]);
					return country_owner;
				}
			}
		}
	}
	print_error("Province id not found:  ", province_id);
	return country_owner;
}
Dictionary CountryData::get_country_from_name(String name)
{
	Dictionary dic;
	for (Dictionary entry_country : country_data)
	{
		if (String(entry_country["Name"]) == name)
		{
			dic = entry_country;
			return dic;
		}
	}
	print_error("Province name not found:  ", name);
	return dic;
}
Dictionary CountryData::get_province_from_id(int32_t id)
{
	Dictionary dic;
	for (Dictionary entry : province_data)
	{
		if (int32_t(entry["Id"]) == id)
		{
			dic = entry;
			return dic;
		}
	}
	print_error("Province id not found:  ", id);
	return dic;
}
void CountryData::change_province_owner(int32_t province_id, const String &new_country_name)
{
	Dictionary country = get_country_from_name(new_country_name);
	// UtilityFunctions::print(get_country_provinces(country_data.find(country)));

	Dictionary province = get_province_from_id(province_id);
	province["Owner"] = country["Id"];
	province_data.set(province_data.find(province), province);
	// UtilityFunctions::print(get_country_provinces(country_data.find(country)));
}
Color CountryData::get_country_color_from_province_id(uint32_t province_id)
{
	// get black
	Color color = Color(0, 0, 0, 1);

	for (int i = 0; i < province_data.size(); i++)
	{
		const Dictionary &entry = province_data[i];
		//find province id
		if (uint32_t(entry["Id"]) == province_id)
		{
			//get its owner it should be as an identifier country like AAC (for Aachen)
			String country_id = entry["Owner"];
			for (const Dictionary &entry_country : country_data)
			{
				//find the country with the identifier
				if (String(entry_country["Id"]) == country_id)
				{
					//using the country name get the color
					color = get_country_color(entry_country["Name"]);
					break;
				}
			}
		}
	}

	return color;
}

String CountryData::parse_province_owner(const String &file_path)
{
	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	String content = file->get_buffer(file->get_length()).get_string_from_ascii();

	String owner_token = "owner =";
	int owner_pos = content.find(owner_token);
	if (owner_pos == -1)
	{
		// handle the mountain case
		String filename = file_path.get_file();
		String assigned_owner = get_terrain_owner(filename);
		if (assigned_owner.is_empty() == false)
		{
			return assigned_owner;
		}

		UtilityFunctions::print(filename + " is unassigned");
		return "No Owner";
	}
	int value_start = owner_pos + owner_token.length();
	int line_end = content.find("\n", value_start);
	//get ID
	String province_owner = content.substr(value_start, line_end - value_start).strip_edges();
	//strip comments
	int comment_pos = province_owner.find("#");
	if (comment_pos == -1)
	{
		comment_pos = province_owner.find("//");
	}
	if (comment_pos != -1)
	{
		province_owner = province_owner.substr(0, comment_pos).strip_edges();
	}
	return province_owner;
}
Color CountryData::parse_country_color(const String &file_path)
{
	Color color;
	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	String content = file->get_buffer(file->get_length()).get_string_from_ascii();

	String owner_token = "color = ";
	int owner_pos = content.find(owner_token);
	int value_start = owner_pos + owner_token.length();
	int line_end = content.find("\n", value_start);
	//get ID
	String province_owner = content.substr(value_start, line_end - value_start).strip_edges();
	PackedStringArray fields = province_owner.split(" ");

	// only add numbers
	Array rgb;

	for (auto field : fields)
	{
		//try to remove non numbers
		field = field.replace("}", "");
		field = field.replace("{", "");
		if (field.is_valid_float())
		{
			rgb.push_back(field.to_int());
		}
	}
	if (rgb.size() != 3)
	{
		UtilityFunctions::print("Could not parse", rgb);
		UtilityFunctions::print("Found Strings", fields);
		UtilityFunctions::print("Could not parse file:", file_path);
		return color;
	}
	color = Color::from_rgba8(rgb[0], rgb[1], rgb[2]);

	return color;
}
Array CountryData::get_country_data() const
{
	return country_data;
}
Color CountryData::get_country_color(const String &country_name)
{
	Color color = Color(1, 1, 1, 1);
	for (const auto &i : country_color_data)
	{
		Dictionary color_entry = i;

		if (country_name == String(color_entry["Name"]))
		{
			// Cast the Variant to Color
			return color = Color(color_entry["Color"]);
		}
	}

	print_error("Country name not found: ", country_name);

	return color;
}

int64_t CountryData::get_country_id_from_name(String name)
{
	for (int64_t i = 0; i < country_color_data.size(); i++)
	{
		Dictionary dict = country_color_data[i];
		if (dict["Name"] == name)
		{
			return i;
		}
	}

	print_error("Did not find country color for: ", name);
	return -1;
}
bool CountryData::set_country_color_by_name(const String &country_name, const Color &new_color)
{
	int64_t index = -1;
	for (int i = 0; i < country_color_data.size(); i++)
	{
		Dictionary dict = country_color_data[i];
		if (dict.get("Name", "") == country_name)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{
		return false;
	}
	Dictionary entry = country_color_data.get(index);

	entry["Color"] = new_color;

	country_color_data.set(index, entry);

	return true;
}

void CountryData::export_color_data(int64_t color_index)
{
	//This rewrites the entire file to not corrupt it
	Dictionary color_data = country_color_data.get(color_index);
	String country_name = color_data["Name"];
	Color country_color = color_data["Color"];
	String file_path = country_colors_folder_path.path_join(country_name + ".txt");

	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	if (!file.is_valid())
	{
		print_error("Could not open file for reading: " + file_path);
		return;
	}

	PackedByteArray raw_bytes = file->get_buffer(file->get_length());
	file->close();

	String content = raw_bytes.get_string_from_ascii();

	PackedStringArray lines = content.split("\n");

	String owner_token = "color = ";
	bool found = false;

	for (int i = 0; i < lines.size(); i++)
	{
		if (lines[i].contains(owner_token))
		{
			int32_t r = country_color.get_r8();
			int32_t g = country_color.get_g8();
			int32_t b = country_color.get_b8();
			String new_color_value = String("{ ") + String::num_int64(r) + " " + String::num_int64(g) + " " + String::num_int64(b) + " }";
			lines[i] = owner_token + new_color_value;
			found = true;
			break;
		}
	}

	if (!found)
	{
		UtilityFunctions::print("Color line not found in: " + country_name);
		return;
	}

	String new_content = "";
	for (int i = 0; i < lines.size(); i++)
	{
		new_content += lines[i];
		if (i < lines.size() - 1)
		{
			new_content += "\n";
		}
	}

	file = FileAccess::open(file_path, FileAccess::WRITE);
	if (!file.is_valid())
	{
		UtilityFunctions::print("Could not open file for writing: " + file_path);
		return;
	}

	file->store_string(new_content);

	UtilityFunctions::print("Successfully updated color for: " + country_name);
}

void CountryData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_provinces_folder", "path"), &CountryData::set_provinces_folder);
	ClassDB::bind_method(D_METHOD("get_provinces_folder"), &CountryData::get_provinces_folder);
	ClassDB::bind_method(D_METHOD("set_countries_folder", "path"), &CountryData::set_countries_folder);
	ClassDB::bind_method(D_METHOD("get_countries_folder"), &CountryData::get_countries_folder);
	ClassDB::bind_method(D_METHOD("set_countries_color_folder", "path"), &CountryData::set_countries_color_folder);
	ClassDB::bind_method(D_METHOD("get_countries_color_folder"), &CountryData::get_countries_color_folder);
	ClassDB::bind_method(D_METHOD("export_color_data"), &CountryData::export_color_data);
	ClassDB::bind_method(D_METHOD("get_country_id_from_name"), &CountryData::get_country_id_from_name);

	ClassDB::bind_method(D_METHOD("get_province_data"), &CountryData::get_province_data);
	ClassDB::bind_method(D_METHOD("set_province_data"), &CountryData::set_province_data);
	ClassDB::bind_method(D_METHOD("parse_all_files"), &CountryData::parse_all_files);
	ClassDB::bind_method(D_METHOD("get_country_provinces"), &CountryData::get_country_provinces);
	ClassDB::bind_method(D_METHOD("get_country_data"), &CountryData::get_country_data);
	ClassDB::bind_method(D_METHOD("get_country_color_data"), &CountryData::get_country_color_data);
	ClassDB::bind_method(D_METHOD("get_country_color", "name"), &CountryData::get_country_color);
	ClassDB::bind_method(D_METHOD("change_province_owner"), &CountryData::change_province_owner);
	ClassDB::bind_method(D_METHOD("get_country_color_from_province_id", "name"), &CountryData::get_country_color_from_province_id);
	ClassDB::bind_method(D_METHOD("populate_color_map_buffers"), &CountryData::populate_color_map_buffers);

	ClassDB::bind_method(D_METHOD("set_country_color_by_name", "name", "color"), &CountryData::set_country_color_by_name);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "provinces_folder", PROPERTY_HINT_DIR), "set_provinces_folder", "get_provinces_folder");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "countries_folder", PROPERTY_HINT_DIR), "set_countries_folder", "get_countries_folder");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "countries_color_folder", PROPERTY_HINT_DIR), "set_countries_color_folder", "get_countries_color_folder");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "map_data"), "set_countries_folder", "get_countries_folder");
}