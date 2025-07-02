#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QLineEdit>

class MonitorWindow : public QWidget {
    Q_OBJECT

public:
    explicit MonitorWindow(QWidget *parent = nullptr);

private:
    QTableWidget *processTable;  // Table for listing processes
    QPushButton *refreshButton;  // Refresh button
    QPushButton *terminateButton;  // Terminate process button
    QLabel *cpuUsageLabel;  // CPU Usage label
    QLabel *memoryUsageLabel;  // Memory Usage label
    QLineEdit *searchBox;  // Search box for filtering processes
    QTimer *updateTimer;  // Timer for updating system resource usage

    void updateProcessList();  // Updates the process list
    void updateResourceUsage();  // Updates CPU and memory usage
    void terminateSelectedProcess();  // Terminates selected process
    void filterProcessList();  // Filters processes based on search
    bool isSuspiciousProcess(const QString &processName);  // Checks if a process is suspicious
};

#endif // MONITORWINDOW_H
