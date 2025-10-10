
#include "godot_cpp/classes/ref_counted.hpp"
namespace godot
{
class CountryInspector : public RefCounted
{
	GDCLASS(CountryInspector, RefCounted);

public:
	CountryInspector() = default;
	~CountryInspector() = default;

private:
	static void _bind_methods();
};
} // namespace godot
