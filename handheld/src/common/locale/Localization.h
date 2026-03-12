#include <vector>
#include <unordered_map>
#include <string>

class AppPlatform;
class ItemInstance;
class ResourcePackManager;

class Localization
{
public:
	std::string getFullLanguageCode() const;
	int getLanguageCode();
	static int getLanguageCode(const std::string &);
	bool getStringIdExists(const std::string &);
	bool get(const std::string &,const std::string &,const std::vector<std::string> &) const;
	bool getCommaSeperator() const;
};