import subprocess

# Run git log command
output = subprocess.check_output(["git", "log", "--numstat"]).decode("utf-8")

# Split output into lines
lines = output.strip().split("\n")

# Initialize variables to keep track of lines added and deleted
total_lines_added = 0
total_lines_deleted = 0
current_commit_lines_added = 0
current_commit_lines_deleted = 0

# Iterate through lines
for line in lines:
    parts = line.split("\t")
    if len(parts) == 3:
        # Check if line contains added or deleted lines count
        added_lines = int(parts[0])
        deleted_lines = int(parts[1])
        file_path = parts[2]
        if file_path.startswith("src/") and added_lines >= 0 and deleted_lines >= 0:
            # Update counters
            current_commit_lines_added += added_lines
            current_commit_lines_deleted += deleted_lines
            total_lines_added += added_lines
            total_lines_deleted += deleted_lines
    elif line.startswith("commit"):
        # Print lines added and deleted in current commit
        print(f"Lines added in current commit: {current_commit_lines_added}")
        print(f"Lines deleted in current commit: {current_commit_lines_deleted}")
        print("-------------------------")
        current_commit_lines_added = 0
        current_commit_lines_deleted = 0

# Print total lines added and deleted
print(f"Total lines added: {total_lines_added}")
print(f"Total lines deleted: {total_lines_deleted}")
