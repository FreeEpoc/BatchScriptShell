#!/usr/bin/env python3
"""
Web scraper to download tutorial web pages from tutorialspoint.com
"""

import os
import re
import requests
from urllib.parse import urljoin, urlparse
from bs4 import BeautifulSoup
import markdownify


def download_tutorial_pages():
    """
    Downloads tutorial pages from https://www.tutorialspoint.com/batch_script/index.htm
    and all pages starting with https://www.tutorialspoint.com/batch_script/
    """
    base_url = "https://www.tutorialspoint.com/batch_script/index.htm"
    visited_urls = set()
    urls_to_visit = [base_url]
    
    # Create directories if they don't exist
    os.makedirs("docs/www", exist_ok=True)
    os.makedirs("docs/imgs", exist_ok=True)
    
    while urls_to_visit:
        current_url = urls_to_visit.pop(0)
        
        if current_url in visited_urls:
            continue
            
        print(f"Processing: {current_url}")
        
        try:
            response = requests.get(current_url)
            response.raise_for_status()
            
            # Save the raw HTML
            filename = os.path.basename(urlparse(current_url).path) or "index.htm"
            with open(f"docs/www/{filename}", "w", encoding="utf-8") as f:
                f.write(response.text)
            
            soup = BeautifulSoup(response.text, 'html.parser')
            
            # Find and download images
            for img_tag in soup.find_all('img'):
                img_src = img_tag.get('src')
                if img_src:
                    img_url = urljoin(current_url, img_src)
                    download_image(img_url)
            
            # Look for the next button with id 'btn_top_next'
            next_button = soup.find('div', id='btn_top_next')
            if next_button:
                # Find the parent link or determine the next page URL
                # Usually, these buttons are within an anchor tag or have a data attribute
                parent_link = next_button.find_parent('a')
                if parent_link and parent_link.get('href'):
                    next_url = urljoin(current_url, parent_link['href'])
                    if next_url.startswith("https://www.tutorialspoint.com/batch_script/"):
                        if next_url not in visited_urls:
                            urls_to_visit.append(next_url)
            
            # Also look for other links within the batch_script domain
            for link in soup.find_all('a', href=True):
                link_url = urljoin(current_url, link['href'])
                if link_url.startswith("https://www.tutorialspoint.com/batch_script/"):
                    if link_url not in visited_urls and link_url not in urls_to_visit:
                        urls_to_visit.append(link_url)
            
            # Convert HTML to markdown
            convert_html_to_markdown(filename)
            
            visited_urls.add(current_url)
            
        except requests.RequestException as e:
            print(f"Error downloading {current_url}: {e}")
            continue
    
    print(f"Downloaded {len(visited_urls)} pages.")


def download_image(img_url):
    """
    Download an image from the given URL and save it to the images directory
    """
    try:
        response = requests.get(img_url)
        response.raise_for_status()
        
        img_filename = os.path.basename(urlparse(img_url).path)
        if not img_filename:
            img_filename = "image_" + str(hash(img_url)) + ".jpg"
        
        # Ensure unique filename
        counter = 1
        original_filename = img_filename
        while os.path.exists(f"docs/imgs/{img_filename}"):
            name, ext = os.path.splitext(original_filename)
            img_filename = f"{name}_{counter}{ext}"
            counter += 1
        
        with open(f"docs/imgs/{img_filename}", "wb") as f:
            f.write(response.content)
        
        print(f"Downloaded image: {img_filename}")
        
        # Update image references in HTML files to point to local images
        update_image_references(original_filename, img_filename)
        
    except requests.RequestException as e:
        print(f"Error downloading image {img_url}: {e}")


def update_image_references(old_filename, new_filename):
    """
    Updates HTML files to reference the local image instead of the remote one
    """
    for html_file in os.listdir("docs/www/"):
        if html_file.endswith(".htm") or html_file.endswith(".html"):
            with open(f"docs/www/{html_file}", "r", encoding="utf-8") as f:
                content = f.read()
            
            # Replace image references
            updated_content = content.replace(old_filename, f"../imgs/{new_filename}")
            
            if updated_content != content:
                with open(f"docs/www/{html_file}", "w", encoding="utf-8") as f:
                    f.write(updated_content)


def convert_html_to_markdown(html_filename):
    """
    Convert HTML file to markdown and save in docs/ directory
    """
    try:
        with open(f"docs/www/{html_filename}", "r", encoding="utf-8") as f:
            html_content = f.read()
        
        soup = BeautifulSoup(html_content, 'html.parser')
        
        # Remove script and style elements
        for script in soup(["script", "style"]):
            script.decompose()
        
        # Extract the main content (usually in a specific div)
        # This might need to be adjusted based on the actual page structure
        main_content = soup.find('div', class_='content') or soup.find('div', id='content') or soup.find('div', class_='intro') or soup
        
        # Convert to markdown
        markdown_content = markdownify.markdownify(str(main_content), heading_style="ATX")
        
        # Create docs directory if it doesn't exist
        os.makedirs("docs", exist_ok=True)
        
        # Change extension to .md
        md_filename = os.path.splitext(html_filename)[0] + ".md"
        
        with open(f"docs/{md_filename}", "w", encoding="utf-8") as f:
            f.write(markdown_content)
        
        print(f"Converted {html_filename} to {md_filename}")
        
    except Exception as e:
        print(f"Error converting {html_filename} to markdown: {e}")


if __name__ == "__main__":
    download_tutorial_pages()