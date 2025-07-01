#!/usr/bin/env python3
import argparse
import subprocess
import zipfile
import shutil
from pathlib import Path
from datetime import datetime

class CodebaseSwitcher:
    STATES = ['preedit', 'beetle', 'sonnet', 'rewrite']
    ACTIVE_STATES = ['beetle', 'sonnet', 'rewrite']
    EXCLUDE_FILES = {'codebase_switcher_mi.py', '.gitignore', 'backup_and_zip.sh', 'extract_backup.sh', 'LICENSE'}
    
    EXCLUDE_PATTERNS = {
        'node_modules', '.npm', '.yarn', 'npm-debug.log', 'yarn-debug.log', 'yarn-error.log',
        '.pnpm-debug.log', '.next', '.nuxt', '.vuepress', 'target', '.gradle', 'bin', 'obj',
        '__pycache__', '.venv', 'venv', 'ENV', '.Python', 'develop-eggs', 'downloads', 
        'eggs', '.eggs', 'lib64', 'parts', 'sdist', 'var', 'wheels',
        '.vscode', '.idea', '.sublime-project', '.sublime-workspace', '.DS_Store', 
        '.Spotlight-V100', '.Trashes', 'ehthumbs.db', 'Thumbs.db', '.cache', '.temp', '.tmp', 'logs'
    }
    
    EXCLUDE_EXTENSIONS = {
        '.pyc', '.pyo', '.class', '.o', '.a', '.lib', '.so', '.dylib', '.dll', '.exe', '.pdb',
        '.log', '.tmp', '.temp', '.swp', '.swo', '.bak', '.backup', '.old', '.orig', '.save',
        '.zip', '.tar', '.tar.gz', '.rar', '.7z', '.bz2', '.xz', '.deb', '.rpm', '.pkg', 
        '.dmg', '.msi', '.jar', '.war', '.ear'
    }
    
    GIT_USER = 'Codebase Switcher'
    GIT_EMAIL = 'switcher@codebase.local'
    
    MESSAGES = {
        'preedit_ready': "✅ Already on preedit baseline branch",
        'preedit_now': "✅ Now on preedit baseline branch",
        'model_ready': "✅ Already on {state} branch\n🤖 Ready for {title} model work",
        'model_now': "✅ Now on {state} branch\n🤖 Ready for {title} model work",
        'rewrite_ready': "✅ Already on rewrite branch\n🔄 Ready for rewrite work",
        'rewrite_now': "✅ Now on rewrite branch\n🔄 Ready for rewrite work"
    }
    
    def __init__(self):
        self.current_dir = Path.cwd()
        self.base_branch = None
        self.keep_branch = None
        self._current_branch_cache = None
        self._git_available = None
        
    def _run_git(self, args, capture=False):
        try:
            result = subprocess.run(['git'] + args, capture_output=capture, text=True, 
                                  check=not capture, cwd=self.current_dir)
            return result.stdout.strip() if capture and result.returncode == 0 else (True if not capture else None)
        except (FileNotFoundError, subprocess.CalledProcessError) as e:
            if not capture and hasattr(e, 'returncode') and e.returncode == 128:
                print(f"❌ Git error: {getattr(e, 'stderr', 'Repository operation failed')}")
            return False if not capture else None
    
    def _validate_git(self):
        if self._git_available is None:
            if not shutil.which('git'):
                print("❌ Git not found in PATH.")
                self._git_available = False
            elif not (self.current_dir / '.git').exists():
                print("❌ Not a git repo. Run --init first.")
                self._git_available = False
            else:
                self._git_available = True
        return self._git_available
    
    def _get_current_branch(self):
        if self._current_branch_cache is None:
            self._current_branch_cache = self._run_git(['branch', '--show-current'], capture=True) or "main"
        return self._current_branch_cache
    
    def _clear_branch_cache(self):
        self._current_branch_cache = None
    
    def _branch_exists(self, branch):
        result = self._run_git(['branch', '--list', branch], capture=True)
        return branch in result if result else False
    
    def _has_changes(self):
        return bool(self._run_git(['status', '--porcelain'], capture=True))
    
    def _should_exclude(self, file_path, zip_name):
        name = file_path.name
        if name in self.EXCLUDE_FILES or name == zip_name or file_path.suffix.lower() in self.EXCLUDE_EXTENSIONS:
            return True
        return any(part in self.EXCLUDE_PATTERNS or name in self.EXCLUDE_PATTERNS or 
                  any(name.startswith(pattern) for pattern in self.EXCLUDE_PATTERNS if not pattern.startswith('.'))
                  for part in file_path.parts)
    
    def _auto_commit(self, suffix=""):
        if not self._has_changes():
            return True
        current = self._get_current_branch()
        print(f"💾 Auto-committing changes on {current}{suffix}")
        return self._run_git(['add', '.']) and self._run_git(['commit', '-m', f'Auto-commit on {current}{suffix}'])
    
    def _handle_commit_before_switch(self, current):
        if not self._has_changes():
            return True
        
        if current in self.ACTIVE_STATES:
            return self._auto_commit(f" - {current} model work")
        elif current == 'preedit':
            print("📝 Switching from baseline (changes will be preserved)")
            return True
        return True
    
    def _switch_branch(self, target):
        print(f"🔄 Switching from {self._get_current_branch()} to {target} branch...")
        if self._run_git(['checkout', target]):
            self._clear_branch_cache()
            return True
        print(f"❌ Failed to switch to {target}")
        return False
    
    def _print_status_message(self, state):
        if state == 'preedit':
            print(self.MESSAGES['preedit_now'])
        elif state == 'rewrite':
            print(self.MESSAGES['rewrite_now'])
        else:
            print(self.MESSAGES['model_now'].format(state=state, title=state.title()))
    
    def _create_gitignore_patterns(self):
        patterns = []
        for pattern in self.EXCLUDE_PATTERNS:
            if pattern.startswith('.'):
                patterns.extend([pattern, f"**/{pattern}"])
            else:
                patterns.extend([f"{pattern}/", f"**/{pattern}/"])
        patterns.extend(f"*{ext}" for ext in self.EXCLUDE_EXTENSIONS)
        return patterns
    
    def initialize(self):
        if not shutil.which('git'):
            print("❌ Git not found in PATH.")
            return False
            
        if not (self.current_dir / '.git').exists():
            print("🔧 Initializing git repository...")
            if not self._run_git(['init']):
                return False
            print("🔧 Setting up local git configuration...")
            self._run_git(['config', '--local', 'user.name', self.GIT_USER])
            self._run_git(['config', '--local', 'user.email', self.GIT_EMAIL])
        
        if not self._run_git(['log', '--oneline', '-1'], capture=True):
            print("📝 Creating initial commit...")
            self._create_initial_files()
            if not (self._run_git(['add', '.']) and 
                   self._run_git(['commit', '-m', 'Initial commit - baseline for model comparison'])):
                print("❌ Failed to create initial commit")
                return False
        
        if not (self._auto_commit(" before branch setup") and self._create_branches()):
            return False
        
        print("✅ Initialized model comparison environment")
        return True
    
    def _create_initial_files(self):
        try:
            readme_content = (f"# Model Comparison Project\n\nProject Date: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n"
                             f"This project contains model comparison results.\n\n"
                             f"- `preedit` branch: Original baseline codebase\n"
                             f"- `beetle` branch: Beetle model's response\n"
                             f"- `sonnet` branch: Sonnet model's response\n"
                             f"- `rewrite` branch: Rewritten codebase\n")
            (self.current_dir / 'README.md').write_text(readme_content, encoding='utf-8')
            
            gitignore_path = self.current_dir / '.gitignore'
            patterns = self._create_gitignore_patterns()
            switcher_section = ("# Auto-generated by codebase switcher\n"
                               "# Excludes large files and build artifacts to keep branches clean\n\n" +
                               '\n'.join(patterns) + "\n")
            
            if gitignore_path.exists():
                existing = gitignore_path.read_text(encoding='utf-8')
                if "# Auto-generated by codebase switcher" not in existing:
                    gitignore_path.write_text(existing.rstrip() + "\n\n" + switcher_section, encoding='utf-8')
                    print("📋 Updated existing .gitignore")
                else:
                    print("📋 .gitignore already contains switcher patterns")
            else:
                gitignore_path.write_text(switcher_section, encoding='utf-8')
                print("📋 Created .gitignore")
        except (OSError, UnicodeError) as e:
            print(f"⚠️  Warning: Could not create initial files: {e}")
    
    def _create_branches(self):
        if not self._branch_exists('preedit'):
            print("📝 Creating preedit branch from preedit")
            if not self._run_git(['checkout', '-b', 'preedit']):
                return False
        elif not self._switch_branch('preedit'):
            return False
        
        for state in ['beetle', 'sonnet']:
            if not self._branch_exists(state):
                print(f"📝 Creating {state} branch from preedit")
                if not self._run_git(['checkout', '-b', state]):
                    return False
            if not self._switch_branch('preedit'):
                return False
        return True
    
    def switch_state(self, state):
        if state not in self.STATES:
            print(f"❌ Invalid state. Available: {', '.join(self.STATES)}")
            return False
        
        if not self._validate_git():
            return False
        
        if state == 'rewrite':
            return self._handle_rewrite()
        
        if not self._branch_exists(state):
            print(f"❌ State '{state}' doesn't exist. Run --init first.")
            return False
        
        current = self._get_current_branch()
        
        if current == state:
            if current in self.ACTIVE_STATES and self._has_changes():
                if not self._auto_commit(f" - {state} model work"):
                    return False
            message = self.MESSAGES['preedit_ready'] if state == 'preedit' else (
                self.MESSAGES['rewrite_ready'] if state == 'rewrite' else 
                self.MESSAGES['model_ready'].format(state=state, title=state.title()))
            print(message)
            return True
        
        if not self._handle_commit_before_switch(current):
            return False
        
        if self._switch_branch(state):
            self._print_status_message(state)
            return True
        return False
    
    def _handle_rewrite(self):
        base_branch = self.base_branch or 'preedit'
        
        if not self._branch_exists(base_branch):
            print(f"❌ Base branch '{base_branch}' doesn't exist. Run --init first.")
            return False
        
        current = self._get_current_branch()
        
        if self._branch_exists('rewrite'):
            if current == 'rewrite':
                if self._has_changes() and not self._auto_commit(" - rewrite work"):
                    return False
                print(self.MESSAGES['rewrite_ready'])
                return True
            
            if not self._handle_commit_before_switch(current):
                return False
            
            return self._switch_branch('rewrite') and (print(self.MESSAGES['rewrite_now']) or True)
        
        print(f"📝 Creating rewrite branch from {base_branch} branch...")
        
        if not self._handle_commit_before_switch(current):
            return False
        
        if current == 'preedit' and self._has_changes():
            if not self._auto_commit(" - baseline changes"):
                return False
        
        if current != base_branch and not self._switch_branch(base_branch):
            return False
        
        if self._run_git(['checkout', '-b', 'rewrite']):
            self._clear_branch_cache()
            print(f"✅ Created rewrite branch from {base_branch}\n🔄 Ready for rewrite work")
            return True
        print("❌ Failed to create rewrite branch")
        return False
    
    def show_status(self):
        if not self._validate_git():
            return
        
        current = self._get_current_branch()
        has_changes = self._has_changes()
        
        print(f"📍 Current branch: {current}")
        print(f"🔄 Available models: {', '.join(['beetle', 'sonnet', 'rewrite'])}")
        
        if current == 'preedit':
            print("💻 On baseline branch (internal)")
            if has_changes:
                print("📝 Baseline has changes (will be preserved when switching)")
        elif current in self.ACTIVE_STATES:
            work_type = "rewrite" if current == 'rewrite' else f"{current.title()}"
            print(f"🤖 On {current} branch - working with {work_type}")
            if has_changes:
                work_desc = "rewrite work" if current == 'rewrite' else "model work"
                print(f"⚡ You have uncommitted {work_desc} (will auto-commit on switch)")
        
        if not has_changes:
            print("✅ All changes committed")
        
        for section, cmd in [("🔄 Git status:", ['status', '--short']), 
                            ("📚 Recent commits:", ['log', '--oneline', '-3'])]:
            print(f"\n{section}")
            result = self._run_git(cmd, capture=True)
            print(result if result else ("No changes" if "status" in section else "No commits found"))
    
    def show_version(self):
        print("v.0630_1800_mi")
    
    def create_zip(self):
        if not self._validate_git():
            return False
        
        current = self._get_current_branch()
        if self._has_changes() and not self._auto_commit(f" - final {current} work"):
            return False
        
        if not self._verify_branches_different():
            return False
        
        print("🔄 Switching to preedit baseline for zip creation...")
        if not self._switch_branch('preedit'):
            return False
        
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        zip_name = f"model_comparison_{self.current_dir.name}_{timestamp}.zip"
        zip_path = self.current_dir / zip_name
        
        print(f"📦 Creating model comparison results: {zip_name}...")
        
        try:
            with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
                for file_path in self.current_dir.rglob('*'):
                    if file_path.is_file() and not self._should_exclude(file_path, zip_name):
                        try:
                            zipf.write(file_path, file_path.relative_to(self.current_dir).as_posix())
                        except (OSError, ValueError) as e:
                            print(f"⚠️  Skipping {file_path.name}: {e}")
            
            print(f"✅ Created {zip_name} ({zip_path.stat().st_size / 1024:.1f} KB)")
            print("📋 Contains: Model comparison results with git history")
            
            final_branch = self.keep_branch or 'preedit'
            if final_branch != 'preedit':
                if self._branch_exists(final_branch):
                    print(f"🔄 Preparing {final_branch} branch content as final codebase...")
                    if not self._switch_branch(final_branch):
                        print(f"⚠️  Warning: Could not switch to {final_branch}, keeping preedit")
                        final_branch = 'preedit'
                else:
                    print(f"⚠️  Warning: Branch '{final_branch}' not found, keeping preedit")
                    final_branch = 'preedit'
            
            print("\n🧹 Starting complete branch cleanup...")
            success = self._cleanup_branches(final_branch)
            
            final_content = final_branch
            print("📊 Zip created and branches cleaned up successfully!\n🎯 Project ready for fresh analysis or archival" 
                  if success else "⚠️  Zip created but branch cleanup had issues\n   You may need to manually clean up remaining branches")
            print(f"📍 Final codebase contains: {final_content} branch content")
            return success
            
        except Exception as e:
            print(f"❌ Error creating zip: {e}")
            if zip_path.exists():
                try:
                    zip_path.unlink()
                    print("🗑️  Cleaned up incomplete zip file")
                except OSError:
                    print("⚠️  Could not clean up incomplete zip file")
            return False
    
    def _verify_branches_different(self):
        print("🔍 Verifying model results are different...")
        
        existing_branches = [state for state in self.ACTIVE_STATES if self._branch_exists(state)]
        
        if len(existing_branches) < 2:
            print(f"❌ Need at least 2 branches for comparison. Found: {', '.join(existing_branches) if existing_branches else 'none'}")
            print("   Complete work on model branches first.")
            return False
        
        print(f"📊 Comparing branches: {', '.join(existing_branches)}")
        
        try:
            all_same = True
            for i, branch1 in enumerate(existing_branches):
                for branch2 in existing_branches[i+1:]:
                    result = subprocess.run(['git', 'diff', '--quiet', branch1, branch2], 
                                          cwd=self.current_dir, capture_output=True, check=False)
                    if result.returncode != 0:
                        all_same = False
                        print(f"✅ {branch1} and {branch2} branches are different")
                    else:
                        print(f"⚠️  Warning: {branch1} and {branch2} branches are identical")
            
            if all_same:
                print("⚠️  All branches are identical - this suggests similar responses")
                print("   This is still valid comparison data")
            else:
                print("✅ Model/rewrite responses show differences - good comparison data")
            return True
            
        except FileNotFoundError:
            print("❌ Git not found")
            return False
    
    def _cleanup_branches(self, keep_content_from=None):
        print("🧹 Cleaning up switcher branches...")
        
        safe_branch = next((branch for branch in ['main', 'master'] if self._branch_exists(branch)), None)
        current_branch = self._get_current_branch()
        
        if not safe_branch:
            print("🔄 Creating main branch as safe branch...")
            if not self._run_git(['checkout', '-b', 'main']):
                print("❌ Failed to create main branch")
                return False
            safe_branch = 'main'
        elif current_branch in self.STATES and (not keep_content_from or current_branch != keep_content_from):
            print(f"🔄 Switching to {safe_branch} branch...")
            if not self._switch_branch(safe_branch):
                return False
        elif current_branch in self.STATES and keep_content_from and current_branch == keep_content_from:

            print(f"🔄 Merging {keep_content_from} content to {safe_branch} branch...")
            if not self._switch_branch(safe_branch):
                return False

            if not self._run_git(['reset', '--hard', keep_content_from]):
                print(f"⚠️  Warning: Could not copy {keep_content_from} content to {safe_branch}")
        elif current_branch != safe_branch:
            print(f"🔄 Switching to {safe_branch} branch...")
            if not self._switch_branch(safe_branch):
                return False
        
        if self._get_current_branch() in self.STATES:
            print("❌ Still on switcher branch, cleanup aborted")
            return False
        
        success = True
        for state in self.STATES:
            if self._branch_exists(state):
                print(f"🗑️  Force deleting {state} branch...")
                if not self._run_git(['branch', '-D', state]):
                    print(f"❌ Failed to delete {state} branch")
                    success = False
                else:
                    print(f"✅ Successfully deleted {state} branch")
        
        remaining = [state for state in self.STATES if self._branch_exists(state)]
        if remaining:
            print(f"❌ Failed to delete branches: {', '.join(remaining)}")
            return False
        
        print("🧹 Cleaning up branch references...")
        self._run_git(['gc', '--prune=now'])
        print("✅ Cleanup complete - all switcher branches removed")
        print(f"📍 Now on {safe_branch} branch with model comparison results")
        return success

def main():
    parser = argparse.ArgumentParser(description='Model Comparison Tool - Experimentation & Comparison')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-i', '--init', action='store_true', help='Initialize model comparison environment')
    group.add_argument('-2', '--beetle', action='store_true', help='Switch to beetle branch (auto-commits model work)') 
    group.add_argument('-3', '--sonnet', action='store_true', help='Switch to sonnet branch (auto-commits model work)')
    group.add_argument('-4', '--rewrite', action='store_true', help='Switch to rewrite branch (auto-commits model work)')
    group.add_argument('-s', '--status', action='store_true', help='Show project status with guidance')
    group.add_argument('-z', '--zip', action='store_true', help='Create model comparison results zip and cleanup branches')
    group.add_argument('-v', '--version', action='store_true', help='Show version')
    
    parser.add_argument('--base-branch', choices=['preedit', 'beetle', 'sonnet'], 
                       help='Base branch for rewrite (default: preedit). Only used with --rewrite.')
    parser.add_argument('--keep-branch', choices=['preedit', 'beetle', 'sonnet', 'rewrite'], 
                       help='Branch content to keep as final codebase (default: preedit). Only used with --zip.')
    
    args = parser.parse_args()
    
    if args.base_branch and not args.rewrite:
        parser.error("--base-branch can only be used with --rewrite")
    
    if hasattr(args, 'keep_branch') and args.keep_branch and not args.zip:
        parser.error("--keep-branch can only be used with --zip")
    
    switcher = CodebaseSwitcher()
    if args.rewrite:
        switcher.base_branch = args.base_branch
    if args.zip:
        switcher.keep_branch = getattr(args, 'keep_branch', None)
    
    actions = {
        'init': switcher.initialize,
        'beetle': lambda: switcher.switch_state('beetle'),
        'sonnet': lambda: switcher.switch_state('sonnet'), 
        'rewrite': lambda: switcher.switch_state('rewrite'),
        'status': switcher.show_status,
        'zip': switcher.create_zip,
        'version': switcher.show_version
    }
    
    for action, func in actions.items():
        if getattr(args, action):
            func()
            break

if __name__ == '__main__':
    main() 