# NEUTRAfloater

An intelligent floating pH management system for maintaining optimal water chemistry in pools and other bodies of water.

## Overview

NEUTRAfloater is an Arduino-based automated solution that continuously monitors and adjusts pH levels in pools and similar water bodies. The system uses an afloat box housing an Arduino Nano with attached solution tanks that automatically disperse sodium carbonate through a peristaltic pump to gradually increase pH toward a target of 7.4.

## Features

- **Real-time pH Monitoring**: Continuous pH level monitoring via integrated sensors
- **Automated Dispensing**: Peristaltic pump automatically dispenses sodium carbonate solution based on real-time measurements
- **Web-Based Control**: Monitor and manage your system through an intuitive web app interface
- **Battery Powered**: Completely cordless operation—ideal for pools and outdoor water bodies
- **Waterproof Design**: Fully sealed IP65 junction box with multiple protection layers
- **Smart Adjustment**: Intelligently increases pH toward target level of 7.4

## Hardware Requirements

- **Microcontroller**: Arduino Nano
- **Pump**: Peristaltic pump for precise liquid dispensing
- **Housing**: IP65 junction box with gasket for waterproofing
- **Sensors**: pH sensor(s) for real-time water chemistry monitoring
- **Connections**: Nylon cable glands for waterproof cable entry
- **Power**: Battery-powered (rechargeable recommended)

## Waterproofing & Environmental Protection

The system employs multiple layers of protection to ensure durability in wet environments:

- IP65-rated junction box with gasket seal
- Nylon cable glands for waterproof connections
- Silicone conformal coating on electronics
- Dielectric grease on connectors
- Neutral-cure RTV silicone for additional sealing

## Installation & Setup

1. Assemble the floating box with all components
2. Fill the solution tank (~1 liter capacity) with sodium carbonate solution
3. Connect the Arduino Nano and pump to power
4. Access the web app to configure your target pH and monitoring settings
5. Deploy in your pool or water body

## Usage

1. **Web App Access**: Connect to the web app interface to monitor real-time pH levels
2. **Automatic Operation**: The system continuously monitors pH and automatically adjusts by dispensing solution as needed
3. **Tank Refills**: Monitor solution tank level and refill as needed (approximately 1 liter capacity)

## Target Applications

- Swimming pools
- Water bodies with pH imbalances
- Any pool environment requiring pH maintenance around 7.4

## Current Status

⚠️ **Alpha Phase**: This project is currently in active development. While the core functionality is operational, additional testing and refinement are ongoing.

## Technical Stack

- **Microcontroller**: Arduino Nano
- **Interface**: Web-based application
- **Pump Control**: Peristaltic pump automation
- **Sensors**: Real-time pH detection

## Limitations & Considerations

- Optimized for pool environments at this stage
- Tank capacity is approximately 1 liter—plan refill schedules accordingly
- Currently in alpha phase—stability improvements are ongoing
- Best suited for freshwater pool applications

## Contributing

Contributions are welcome! Whether you have suggestions for improvements, bug reports, or feature requests, please feel free to open an issue or submit a pull request.

## License

[Add your license information here]

## Support

For questions, issues, or feedback about NEUTRAfloater, please open a GitHub issue in this repository.

---

**Made with ⚗️ for cleaner water**
