#include <sstream>
#include <cstdio>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "FF4ArchiveVisitors.h"
#include "FF4ArchiveNode.h"
#include "FF4ArchiveNodeImpl.h"
#include "FF4Exceptions.h"
#include "FF4Utils.h"

ff4psp::ArchiveNode* ff4psp::impl::NodeRetrieveVisitor::visit(ArchiveNode* root, const std::string& path)
{
	if(!isValidPath(path))
		FF4Exception::raise("The given path is not a valid path for the archive!");

	std::istringstream ss(path);

	std::string element;
	const ArchiveNode* node=root;

	while(getline(ss,element,'/'))
	{
		bool found=false;
		for(size_t i=0;i<node->size();i++)
			if(node->getChild(i)->getName()==element)
			{
				node=node->getChild(i);
				found=true;
				break;
			}
		if(!found) return nullptr;
	}

	return const_cast<ArchiveNode*>(node);
}

bool ff4psp::impl::NodeRetrieveVisitor::isValidPath(const std::string& path) const
{
	return true;
}

void ff4psp::impl::ImportVisitor::visit(ArchiveNode* node, const std::string& inputDirectory)
{
	boost::filesystem::path path=ff4psp::utils::encodings::path(inputDirectory);

	_visit(node,path);

}


void ff4psp::impl::ImportVisitor::_visit(ArchiveNode* current, const boost::filesystem::path& inputPath)
{
	if(current->isFile() != boost::filesystem::is_regular_file(inputPath))
		FF4Exception::raise("Cannot import a directory into a file and viceversa!");

	if(current->isFile())
	{
		current->setSourceFile(inputPath.string());
		return;
	}

	for(size_t i=0;i<current->size();i++)
	{
		ArchiveNode* child=current->getChild(i);

		boost::filesystem::path fullPath = inputPath / child->getName();
		//std::unique_ptr<ArchiveNodeImpl> child(new ArchiveNodeImpl());

		//ArchiveNodeImpl* ptr=child.get();
		//current->addChild(std::move(child));

		if(boost::filesystem::exists(fullPath))
		{
			_visit(child,fullPath);
		}
	}
}
