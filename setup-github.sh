#!/bin/bash

# GitHub Repository Setup Script for Localify Android C++
# This script helps you create and push the repository to GitHub

echo "🚀 Setting up Localify Android C++ GitHub Repository"
echo "=================================================="

# Check if git is initialized
if [ ! -d ".git" ]; then
    echo "❌ Git repository not found. Please run 'git init' first."
    exit 1
fi

# Get GitHub username
read -p "Enter your GitHub username: " GITHUB_USERNAME

# Get repository name (default: localify-android-cpp)
read -p "Enter repository name [localify-android-cpp]: " REPO_NAME
REPO_NAME=${REPO_NAME:-localify-android-cpp}

# Check if GitHub CLI is installed
if command -v gh &> /dev/null; then
    echo "✅ GitHub CLI found. Creating repository..."
    
    # Create repository using GitHub CLI
    gh repo create "$REPO_NAME" --public --description "Pure C++ Android app for Localify - Zero Java/XML dependencies" --clone=false
    
    # Add remote origin
    git remote add origin "https://github.com/$GITHUB_USERNAME/$REPO_NAME.git"
    
    echo "✅ Repository created successfully!"
else
    echo "⚠️  GitHub CLI not found. Please create the repository manually:"
    echo "   1. Go to https://github.com/new"
    echo "   2. Repository name: $REPO_NAME"
    echo "   3. Description: Pure C++ Android app for Localify - Zero Java/XML dependencies"
    echo "   4. Make it public"
    echo "   5. Don't initialize with README (we already have one)"
    echo ""
    echo "   Then run:"
    echo "   git remote add origin https://github.com/$GITHUB_USERNAME/$REPO_NAME.git"
    
    read -p "Press Enter after creating the repository on GitHub..."
    
    # Add remote origin
    git remote add origin "https://github.com/$GITHUB_USERNAME/$REPO_NAME.git"
fi

# Push to GitHub
echo "📤 Pushing to GitHub..."
git push -u origin main

echo ""
echo "🎉 Success! Your repository is now available at:"
echo "   https://github.com/$GITHUB_USERNAME/$REPO_NAME"
echo ""
echo "📋 Next steps:"
echo "   1. Add repository topics: android, cpp, ndk, localify, music, native-app"
echo "   2. Enable GitHub Actions for CI/CD"
echo "   3. Add collaborators if needed"
echo "   4. Set up branch protection rules"
echo ""
echo "🔧 To build the project:"
echo "   ./gradlew assembleDebug"
echo ""
echo "📖 Documentation:"
echo "   - README.md - Project overview and setup"
echo "   - CONTRIBUTING.md - Contribution guidelines"
echo "   - .github/ - Issue templates and workflows"
