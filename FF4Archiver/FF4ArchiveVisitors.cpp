#include <sstream>
#include <cstdio>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "FF4ArchiveVisitors.h"
#include "FF4ArchiveNode.h"
#include "FF4ArchiveNodeImpl.h"
#include "FF4Exceptions.h"
#include "FF4Utils.h"

ff4psp::ArchiveNode* ff4psp::impl::NodeRetrieveVisitor::visit(const ArchiveNode* root,const std::string& path)
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

std::unique_ptr<ff4psp::ArchiveNode> ff4psp::impl::ImportVisitor::visit(const ArchiveNode* node,const std::string& inputDirectory)
{
	boost::filesystem::path path=ff4psp::utils::encodings::path(inputDirectory);
	
	std::unique_ptr<ArchiveNodeImpl> current(new ff4psp::impl::ArchiveNodeImpl());

	_visit(node,current.get(),path);

	return std::move(current);

	//
	//long delta=0;
	//boost::filesystem::path tempDir=boost::filesystem::temp_directory_path();
	//std::string tempFile=boost::filesystem::unique_path(tempDir/"%%%%-%%%%-%%%%-%%%%").string();

	//boost::filesystem::ofstream tmpStream(tempFile,std::ios_base::out| std::ios_base::binary);
	//if(!tmpStream)
	//	FF4Exception::raise("Cannot open temp archive file!");

	//const ArchiveNode* root=node;
	//while(!root->isRoot())
	//	root=root->getParent();

	////std::shared_ptr<ArchiveNode> newroot=ArchiveNode::deepCopy(root);

	//boost::filesystem::path inputDir = ff4psp::utils::encodings::path(inputDirectory);
	////return _visit(root,node,inputDir,delta);

}

void ff4psp::impl::ImportVisitor::_visit(const ArchiveNode* original, ArchiveNodeImpl* current, const boost::filesystem::path& inputPath)
{
	if(original->isFile() != boost::filesystem::is_regular_file(inputPath))
		FF4Exception::raise("Cannot import a directory into a file and viceversa!");

	current->setName(original->getName());
	current->setIsFile(original->isFile());

	if(original->isFile())
	{
		current->setSource(inputPath.string());
		current->setFileSize(boost::filesystem::file_size(inputPath));
		return;
	}

	for(size_t i=0;i<original->size();i++)
	{
		const ArchiveNode* temp=original->getChild(i);

		boost::filesystem::path fullPath=inputPath/temp->getName();
		std::unique_ptr<ArchiveNodeImpl> child(new ArchiveNodeImpl());

		ArchiveNodeImpl* ptr=child.get();
		current->addChild(std::move(child));

		if(!boost::filesystem::exists(fullPath))
		{
			ptr->setName(temp->getName());
			ptr->setChecksum(temp->getChecksum());
			ptr->setFileSize(temp->getFileSize());
			ptr->setIsFile(temp->isFile());
			ptr->setSource(dynamic_cast<const ArchiveNodeImpl*>(temp)->getSourceName());
		}else
		{
			_visit(temp,ptr,fullPath);
		}
	}
}
