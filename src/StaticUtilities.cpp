// Marcus Schmidt
// Created on 4/16/23

#include "StaticUtilities.h"
#include <wx/dirdlg.h>
#include <iostream>

std::string StaticUtilities::ChooseDirectory(wxFrame *parent, std::string message)
{
	wxDirDialog *dialog = new wxDirDialog(parent,
								message,
								"",
								wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dialog->ShowModal() == wxID_OK)
		return dialog->GetPath().utf8_string();
	else
		return "";
}

std::vector<std::string> StaticUtilities::RecurseGetImages(std::filesystem::path path)
{
	std::vector<std::string> images;

	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			std::vector<std::string> recursedImages = RecurseGetImages(entry.path());
			images.insert(images.end(), recursedImages.begin(), recursedImages.end());
		}
		else if (entry.is_regular_file())
			images.push_back(entry.path().filename().string());
	}
	
	return images;
}

std::vector<File> StaticUtilities::RecurseGetImages(std::filesystem::path path, SortMethod sortMethod, const std::vector<ToggledString> &fileTypes, Directory *directory)
{
	std::vector<File> images;

	// Confirm the provided directory still exists in its expected location
	if (std::filesystem::is_directory((std::filesystem::path) (path.string() + "/" + directory->name)))
	{
		// Gather information about the filter on this directory that will be passed along
		//*************************************************************************************************
		// NOTE: This work is unused in one of the logical paths but avoids repeating code. Improve this. *
		//*************************************************************************************************
		std::unordered_map<std::string, int> filterItems;
		bool defaultValidity = true;
		if (directory->filter != nullptr && directory->filter->GetType() != Filter::NONE)
		{
			filterItems = directory->filter->GetFilterItems();
			if (directory->filter->GetType() == Filter::INCLUDE)
				defaultValidity = false;
		}

		// If it has nested subdirectories in the code
		if (!directory->subdirectories.empty())
		{
			// Recurse to get their images
			for (auto subdirectory : directory->subdirectories)
			{
				std::vector<File> recursedImages = RecurseGetImages(path.string() + "/" + directory->name, sortMethod, fileTypes, &subdirectory);
				StaticUtilities::MergeVectors(images, recursedImages, sortMethod);
			}

			// And then get the images from this directory if it's active
			if (directory->active)
			{
				for (const auto &entry : std::filesystem::directory_iterator(path))
				{
					if (entry.is_regular_file() && IsValidExtension(fileTypes, entry.path().extension().string()))
						ProcessImage(entry, images, sortMethod, filterItems, defaultValidity);
				}
			}
		}
		// If it does not have nested subdirectories in the code but it is activated
		else if (directory->active)
		{
			// And then recurse with a different method
			std::vector<File> recursedImages = RecurseGetImages(path.string() + "/" + directory->name, sortMethod, fileTypes, filterItems, defaultValidity);
			StaticUtilities::MergeVectors(images, recursedImages, sortMethod);
		}
	}
	else
		std::cout << "RecurseGetImages(): Directory \"" << (path.string() + "/" + directory->name) << "\" does not exist" << std::endl;
	
	return images;
}

std::vector<File> StaticUtilities::RecurseGetImages(std::filesystem::path path, SortMethod sortMethod, const std::vector<ToggledString> &fileTypes, const std::unordered_map<std::string, int> &filterItems, const bool defaultValidity)
{
	std::vector<File> images;

	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			std::vector<File> recursedImages = RecurseGetImages(entry.path(), sortMethod, fileTypes, filterItems, defaultValidity);
			StaticUtilities::MergeVectors(images, recursedImages, sortMethod);
		}
		else if (entry.is_regular_file() && IsValidExtension(fileTypes, entry.path().extension().string()))
			ProcessImage(entry, images, sortMethod, filterItems, defaultValidity);
	}
	
	return images;
}

int StaticUtilities::GetUniqueId()
{
	static int id = 0;
	return id++;
}

/* A standardized image name means two things in this context:
 * 1. All lowercase letters.
 * 2. If this filename ends with ' copy' or ' copy X' before the extension, remove it.
 *    The benefit is to be able to recognize copied images in different folders so that
 *    the filter will work properly without having to manually rename copied images to match. */
std::string StaticUtilities::StandardizeImageName(std::string name)
{
	std::size_t pos = name.rfind(".");
	if (pos == std::string::npos)
		pos = name.size();

	if (pos > 5 && name.substr(pos - 5, 5).compare(" copy") == 0)
		name.erase(pos - 5, 5);
	else if (pos > 7 && name.substr(pos - 7, 6).compare(" copy ") == 0)
		name.erase(pos - 7, 7);

	for (int i = 0; i < name.size(); i++)
		name[i] = tolower(name[i]);

	return name;
}

// Merge vector 'b's elements into vector 'a' using the current sort method (based on merge sort algorithm)
void StaticUtilities::MergeVectors(std::vector<File> &a, const std::vector<File> &b, const SortMethod sortMethod)
{
	if (b.empty())
		return;

	// Reserve enough space for the final vector so that it won't be reallocated and invalidate the iterator
	a.reserve(a.size() + b.size());

	std::vector<File>::iterator iterator = a.begin();
	int i = 0;
	while (iterator < a.end() && i < b.size())
	{
		// If the next element in vector 'b' is smaller according to the current sort method, insert it in the next spot in vector 'a'
		if ((sortMethod == SortMethod::NAME && iterator->name.compare(b[i].name) > 0) ||
		    (sortMethod == SortMethod::DATE && iterator->modifiedTime > b[i].modifiedTime) ||
			(sortMethod == SortMethod::RANDOM && std::rand() % 2 == 1))
		{
			a.insert(iterator, b[i]);
			i++;
		}

		// Move to the next element in vector 'a' (either because it was smaller or because the new element from vector 'b' needs to be skipped)
		iterator++;
	}

	// If the end of vector 'a' has been reached but there are still elements in 'b', add them all to the end of 'a'
	while (i < b.size())
	{
		a.push_back(b[i]);
		i++;
	}
}

template <typename T>
void StaticUtilities::SortAlphabetically(std::vector<T> &vector, T &element)
{
	typename std::vector<T>::iterator it;
	for (it = vector.begin(); it < vector.end(); it++)
	{
		// If the current element in the list is alphabetically greater, break so the new element is inserted here
		if ((*it).name.compare(element.name) > 0)
			break;
	}

	vector.insert(it, element);
}

/* Defining a template function in the .cpp file means it won't be translatable by other classes that include the header file,
   so it either needs to be defined in the header file or explicitly instantiated for all desired types in the .cpp file, as below.
 * Source: https://stackoverflow.com/a/488989 */
template void StaticUtilities::SortAlphabetically(std::vector<Directory> &vector, Directory &element);

//********************
// Private functions *
//********************

template <typename T>
void StaticUtilities::SortRandomly(std::vector<T> &vector, T &element)
{
	if (vector.size() > 0)
	{
		typename std::vector<T>::iterator it = vector.begin();

		// Reference: https://en.cppreference.com/w/cpp/numeric/random/rand
		std::srand(std::time(nullptr));
		int index = std::rand() % vector.size();

		std::advance(it, index);
		vector.insert(it, element);
	}
	else
		vector.push_back(element);
}

void StaticUtilities::SortByTime(std::vector<File> &vector, File &element)
{
	std::vector<File>::iterator it;
	for (it = vector.begin(); it < vector.end(); it++)
	{
		// If the current element in the list was modified later, break so the new element is inserted here
		if ((*it).modifiedTime > element.modifiedTime)
			break;
	}

	vector.insert(it, element);
}

void StaticUtilities::ProcessImage(const std::filesystem::directory_entry &file, std::vector<File> &vector, SortMethod sortMethod, const std::unordered_map<std::string, int> &filterItems, const bool defaultValidity)
{
	std::string name = file.path().filename().string();
	std::string nameStandardized = StaticUtilities::StandardizeImageName(name);
	std::string path = file.path().string().substr(0, file.path().string().length() - name.length());

	// If there is a filter, check whether this image is in the filter and flip its accept state if it's present.
	// Standardize image name before comparing with filter so names like "image.jpg" and "image copy 2.jpg" will evaluate to equal
	bool valid = defaultValidity;
	if (filterItems.find(nameStandardized) != filterItems.end())
		valid = !valid;

	if (valid)
	{
		// Struct documentation: https://man7.org/linux/man-pages/man0/sys_stat.h.0p.html
		struct stat info;
		time_t time;
		// Function documentation: https://pubs.opengroup.org/onlinepubs/007908799/xsh/stat.html
		if (stat(file.path().c_str(), &info) == 0)
		{
			time = info.st_mtime;
		}
		else
		{
			std::cout << "ProcessImage(): Unable to obtain time for \"" << path << "\"" << std::endl;
			time = 0;
		}

		File image = { nameStandardized, name, path, time };
		if (sortMethod == SortMethod::NAME)
			SortAlphabetically(vector, image);
		else if (sortMethod == SortMethod::DATE)
			SortByTime(vector, image);
		else if (sortMethod == SortMethod::RANDOM)
			SortRandomly(vector, image);
		else
		{
			std::cout << "ProcessImage(): Unknown sorting method" << std::endl;
			vector.push_back(image);
		}
	}
}

bool StaticUtilities::IsValidExtension(const std::vector<ToggledString> &fileTypes, std::string extension)
{
	// Convert the extension to all lowercase characters
	for (int i = 0; i < extension.size(); i++)
		extension[i] = tolower(extension[i]);

	// Compare the file's lowercase extension against the active extensions
	for (auto type : fileTypes)
	{
		if (type.active && extension.compare(type.name) == 0)
			return true;
	}

	return false;
}