#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filelistitemmodel.h"
#include "downloadclient.h"
#include <QTcpSocket>
#include <QFile>
#include <QQueue>
#include <QHash>
#include <QThread>
#include "../uibundle.h"

namespace Ui {
class MainWindow;
}

//Forward class declarations
class QSettings;

class ClientUIBundle : public UIBundle
{
public:
	ClientUIBundle();
	ClientUIBundle(FileInfo* file, DownloadClient *clientObj, QWidget *parent = 0);
	~ClientUIBundle();

	void update(qint64 value, double speed);

	void setAborted();
	void setResumed();

private:

	FileInfo *file;
	DownloadClient *client;
};

class DownloadWorkerBundle
{
public:
	DownloadWorkerBundle(): thread(NULL), client(NULL), ui(NULL) { }
	~DownloadWorkerBundle()
	{
		if(ui) delete ui;

		if(thread)
		{
			thread->quit();
			thread->wait();
		}

		if(client) delete client;
		if(thread) delete thread;
	}

	QThread *thread;
	DownloadClient *client;
	ClientUIBundle *ui;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void closeEvent(QCloseEvent *event);

signals:
	void cleanUpThreads();

public slots:
	void downloadFileList();
	void requestFileDownload();
	void selectNewSaveDirectory();

	void fileTransferStarted(FileInfo* file, DownloadClient* dc);
	void fileTransferUpdated(qint64 bytes, double speed, DownloadClient *dc);
	void fileTransferCompleted(DownloadClient *dc);
	void fileTransferAborted(qint64 bytes_recieved, DownloadClient *dc);
	void fileTransferResumed(DownloadClient *dc);

	void removeDownloadUI();


	void sock_connected();
	void sock_error(QAbstractSocket::SocketError err);
	void sock_disconn();
	void onListReceiveData();

private:
	bool getServerAddress(QHostAddress *addr);

    Ui::MainWindow *ui;
	FileListItemModel *tableModel;

	QSettings *settings;

	QTcpSocket *m_socket;

	QFile *out_file;
	FileInfo *rec_file;
	qint64 rec_bytes;

	bool list_ack_receieved;
	int m_items_received;
	int m_items_total;

	QHash<DownloadClient*,DownloadWorkerBundle*> workerHash;
	QQueue<DownloadWorkerBundle*> toRemove;
};

#endif // MAINWINDOW_H
