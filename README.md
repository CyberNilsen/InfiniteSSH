# InfiniteSSH

A simple SSH honeypot written in C that traps potential attackers in an endless loop while logging their attempts.

## What it does

InfiniteSSH creates a fake SSH server that:
- Presents a realistic SSH banner to attackers
- Continuously prompts for passwords without ever authenticating
- Sends fake SSH debug messages to maintain the illusion
- Wastes attackers' time with delays and loops
- Logs all connection attempts and data to console and file

## Features

- **Realistic SSH simulation** - Uses proper SSH banner format
- **Infinite loop trap** - Keeps attackers engaged without giving access
- **Comprehensive logging** - Records all attempts to both console and `honeypot.log`
- **Time wasting** - Built-in delays to waste attackers' time
- **Windows compatible** - Uses Winsock for Windows networking
- **Automatic cleanup** - Disconnects after 50 interactions to prevent resource exhaustion

## Requirements

- Windows operating system
- GCC compiler or Visual Studio
- Administrative privileges (for binding to network ports)

## Installation

1. Clone or download this repository
2. Compile the source code:
   ```cmd
   gcc -o infinitessh.exe infinitessh.c -lws2_32
   ```
   Or with Visual Studio:
   ```cmd
   cl infinitessh.c ws2_32.lib
   ```

## Usage

1. Run the executable:
   ```cmd
   infinitessh.exe
   ```

2. The honeypot will start listening on port 2222

3. Monitor the console output for connection attempts

4. Check `honeypot.log` for persistent logging

## Testing

Test your honeypot using any of these methods:

### PowerShell One-liner
```powershell
$client = New-Object System.Net.Sockets.TcpClient; $client.Connect("127.0.0.1", 2222); $stream = $client.GetStream(); $buffer = New-Object byte[] 1024; $bytesRead = $stream.Read($buffer, 0, 1024); Write-Host ([System.Text.Encoding]::ASCII.GetString($buffer, 0, $bytesRead)); $stream.Close(); $client.Close()
```

### Check if port is listening
```powershell
Test-NetConnection -ComputerName localhost -Port 2222
```

### Using PuTTY or other SSH clients
Connect to `localhost:2222` and observe the trapping behavior.

## How the trap works

1. **Initial Connection**: Sends realistic SSH-2.0 banner
2. **Loop Mechanics**: 
   - Every 3rd interaction: Prompts for password
   - Every 5th interaction: Shows "Authentication failed"
   - Other interactions: Shows fake SSH debug messages
3. **Time Wasting**: 2-second delay between each response
4. **Safety Limit**: Automatically disconnects after 50 interactions

## Example Output

```
InfiniteSSH Honeypot starting on port 2222...
Honeypot listening... Press Ctrl+C to stop.
[Tue Aug  5 12:41:13 2025] Connection from 192.168.1.100: Connected - sent SSH banner
[Tue Aug  5 12:41:15 2025] Connection from 192.168.1.100: admin
[Tue Aug  5 12:41:18 2025] Connection from 192.168.1.100: password123
[Tue Aug  5 12:41:21 2025] Connection from 192.168.1.100: root
```

## Log Files

All activity is logged to `honeypot.log` in the same directory:
```
[Tue Aug  5 12:41:13 2025] 192.168.1.100: Connected - sent SSH banner
[Tue Aug  5 12:41:15 2025] 192.168.1.100: admin
[Tue Aug  5 12:41:18 2025] 192.168.1.100: password123
```

## Security Considerations

- **Use port 2222** instead of 22 to avoid conflicts with real SSH
- **Run in isolated environment** for safety
- **Monitor logs regularly** for analysis
- **Educational use only** - ensure proper authorization before deployment

## Configuration

Key settings in the source code:
- `PORT 2222` - Change the listening port
- `BUFFER_SIZE 1024` - Adjust buffer size for messages
- `loop_count > 50` - Modify disconnect threshold
- `Sleep(2000)` - Adjust delay between responses (milliseconds)

## Learning Objectives

This project helps understand:
- Socket programming in C
- Network protocol simulation
- Windows networking with Winsock
- Basic cybersecurity concepts
- Honeypot design principles

## License

This project is for educational purposes only. Use responsibly and ensure you have proper authorization before deploying any honeypot system.

## Contributing

Feel free to submit issues and pull requests to improve the honeypot's functionality or add new features.
