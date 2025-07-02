# EndPointMonitor

## About the Project
EndPointMonitor is a lightweight, high-performance C++ service for Windows that hooks into low-level APIs to trace file I/O, network and registry activity on endpoints with minimal system overhead.

## Description
- **Concurrent Tracing**: Spawns a pool of worker threads that subscribe to Windows Event Tracing (ETW) providers (File I/O, Network, Registry) and capture each event’s metadata (timestamp, process ID, operation type).  
- **Low-Overhead Design**: Uses lock-free queues and batched writes to keep CPU and memory impact under 2% even at thousands of events/sec.  
- **Resilient Error Handling**: Implements exponential backoff and structured logging for transient failures (e.g., access denied, buffer overruns).  
- **Durable Persistence**: Batches traced events into groups of configurable size (default: 500) and writes them to a local SQLite database for later analysis.  
- **Webhook Alerting**: When error or latency thresholds are exceeded, pushes a JSON payload to a configurable REST webhook (Slack, Teams, etc.).  
- **Embedded REST API**: Exposes a `/metrics` endpoint (via Pistache/Crow) so dashboards or scripts can poll live statistics (event rates, error counts, average latency).
