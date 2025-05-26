import os
import shutil
import time
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import sys

class PluginSyncHandler(FileSystemEventHandler):
    def __init__(self, src_dir, dest_dir):
        self.src_dir = src_dir
        self.dest_dir = dest_dir
        self.ignore_extensions = ['.temp']
        self.ignore_folders = ['Intermediate', 'Binaries']
        self.last_synced = {}
        self.sync_delay = 1  # 1-second delay to avoid syncing incomplete files

        self.initial_sync()

    def initial_sync(self):
        for root, dirs, files in os.walk(self.src_dir):
            dirs[:] = [d for d in dirs if d not in self.ignore_folders]
            for file in files:
                src_file = os.path.join(root, file)
                if any(src_file.endswith(ext) for ext in self.ignore_extensions):
                    continue
                self.sync_files(src_file)

    def on_modified(self, event):
        if not event.is_directory:
            time.sleep(self.sync_delay)  # Wait before syncing to avoid active files
            self.sync_files(event.src_path)

    def on_created(self, event):
        if not event.is_directory:
            time.sleep(self.sync_delay)  # Wait before syncing to avoid active files
            self.sync_files(event.src_path)

    def sync_files(self, src_path):
        if any(src_path.endswith(ext) for ext in self.ignore_extensions):
            return

        if any(folder in src_path for folder in self.ignore_folders):
            return

        now = time.time()
        if src_path in self.last_synced and now - self.last_synced[src_path] < 1:
            return
        self.last_synced[src_path] = now

        relative_path = os.path.relpath(src_path, self.src_dir)
        dest_path = os.path.join(self.dest_dir, relative_path)

        os.makedirs(os.path.dirname(dest_path), exist_ok=True)

        shutil.copy2(src_path, dest_path)
        print(f"Synced: {src_path} to {dest_path}")

def validate_directory_and_plugin(src_dir, dest_dir):
    if not os.path.exists(src_dir):
        print(f"Error: Source directory '{src_dir}' does not exist.")
        sys.exit(1)
    
    if not os.path.exists(dest_dir):
        print(f"Error: Destination directory '{dest_dir}' does not exist.")
        sys.exit(1)

    # Check if .uplugin file exists in the source directory
    plugin_file = os.path.join(src_dir, "MultiplayerPlugin.uplugin")
    if not os.path.isfile(plugin_file):
        print(f"Error: Plugin file '{plugin_file}' not found in the source directory.")
        sys.exit(1)

def monitor_directory(src_dir, dest_dir):
    validate_directory_and_plugin(src_dir, dest_dir)

    event_handler = PluginSyncHandler(src_dir, dest_dir)
    observer = Observer()
    observer.schedule(event_handler, path=src_dir, recursive=True)
    observer.start()
    print(f"Monitoring {src_dir} for changes...")

    try:
        while True:
            time.sleep(1)  # Add a small sleep to reduce CPU usage
    except KeyboardInterrupt:
        observer.stop()
    observer.join()

if __name__ == "__main__":
    src_dir = r"C:\Users\PREDATOR\OneDrive\Documents\Unreal Projects\OnlinePlugin\Plugins\MultiplayerPlugin"
    dest_dir = r"C:\Users\PREDATOR\OneDrive\Documents\Unreal Projects\MyZombies\Source\MyZombies"

    print("Starting the sync script...")
    print(f"Source Directory: {src_dir}")
    print(f"Destination Directory: {dest_dir}")
    monitor_directory(src_dir, dest_dir)