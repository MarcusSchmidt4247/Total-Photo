// Marcus Schmidt
// Created on 4/16/23

#ifndef STATIC_UTILITIES
#define STATIC_UTILITIES

#include "CustomTypes.h"
#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <wx/frame.h>

class StaticUtilities
{
public:
	// Get information from user
	static std::string ChooseDirectory(wxFrame *parent, std::string message);

	// Functions to perform a recursive image search from a directory or filepath
	static std::vector<std::string> RecurseGetImages(std::filesystem::path path); // gathers all image names without discretion, used only for filters
	static std::vector<File> RecurseGetImages(std::filesystem::path path, SortMethod sortMethod, const std::vector<FileType> &fileTypes, Directory *directory); // gathers images and recurses based on directory information
	static std::vector<File> RecurseGetImages(std::filesystem::path path, SortMethod sortMethod, const std::vector<FileType> &fileTypes, const std::unordered_map<std::string, int> &filterItems, const bool defaultValidity); // continues from where directory info ends

	// Miscellaneous functions performing standardized or shared operations
	static int GetUniqueId();
	static std::string StandardizeImageName(std::string name);
	static void MergeVectors(std::vector<File> &a, const std::vector<File> &b, const SortMethod sortMethod);
	static void ProcessImage(const std::filesystem::directory_entry &file, std::vector<File> &vector, SortMethod sortMethod, const std::vector<FileType> &fileTypes, const std::unordered_map<std::string, int> &filterItems, const bool defaultValidity);
	static bool IsActiveExtension(const std::vector<FileType> &fileTypes, std::string extension);

	template <typename T> static void SortAlphabetically(std::vector<T> &vector, T &element);

private:
	template <typename T> static void SortRandomly(std::vector<T> &vector, T &element);
	static void SortByTime(std::vector<File> &vector, File &element);
	static MediaType GetMediaType(const std::vector<FileType> &fileTypes, std::string extension);
};

#endif