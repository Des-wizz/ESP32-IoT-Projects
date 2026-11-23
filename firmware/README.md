# Firmware

Store project firmware in a separate subfolder for each device or application.

Suggested layout:

```text
firmware/
  project-name/
    project-name.ino
    README.md
    src/
    include/
    lib/
```

## Firmware documentation checklist

For every project, document:

- Target board and exact module variant
- Arduino IDE or PlatformIO version
- Required libraries and versions, where known
- GPIO assignments and peripheral interfaces
- Build and upload instructions
- Configuration required before compiling
- Known limitations and test results

Do not commit real credentials or API keys. Keep private settings in a local secrets file and provide a safe example template when needed.
