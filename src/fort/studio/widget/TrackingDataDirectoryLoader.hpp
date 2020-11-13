#pragma once

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>

#include <atomic>

class QProgressDialog;

class TrackingDataDirectoryLoader : public QObject{
	Q_OBJECT
public:
	static void EnsureLoaded(const std::vector<fmp::TrackingDataDirectory::Ptr> & tdds,
	                         QWidget * parent);

	virtual ~TrackingDataDirectoryLoader();


signals:
	void valueChanged(int);

private:
	TrackingDataDirectoryLoader(const std::vector<fmp::TrackingDataDirectory::Ptr> & tdds,
	                            QWidget * parent);

	size_t size() const;

	bool isEmpty() const;


	void append(const fmp::TrackingDataDirectory::Ptr & tdd);

	void initDialog(QWidget * parent);
	void loadAll();
	void load(const std::vector<fmp::TrackingDataDirectory::Loader> & loaders,
	          const QString & caption);

	std::vector<fmp::TrackingDataDirectory::Loader> d_tagCloseUps,
		                                            d_tagStatistics,
		                                            d_fullFrames;
	QProgressDialog * d_dialog;
};
