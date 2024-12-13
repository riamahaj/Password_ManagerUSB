import csv
import random
import string

# Function to generate random password
def generate_random_password(length=12):
    characters = string.ascii_letters + string.digits + string.punctuation
    return ''.join(random.choice(characters) for i in range(length))

# List of example websites
websites = [
    "example.com", "example.org", "website1.com", "website2.net", "demo.co", "testsite.com", "mywebsite.org",
    "samplesite.net", "anotherexample.com", "yoursite.org", "randomsite.com", "coolwebsite.net", "blogexample.org",
    "newswebsite.com", "companysite.net", "portalexample.org", "yourdomain.com", "goodwebsite.net", "somewebsite.com",
    "safesite.org", "webportal.net", "mystore.com", "onlinehub.org", "gamehub.net", "appsite.com", "techcompany.org",
    "musicstream.com", "streamingwebsite.net", "moviesite.org", "photoapp.com", "socialhub.net", "fitnessapp.org",
    "jobportal.com", "onlineshop.net", "ecommercehub.org", "schoolportal.com", "librarysite.net", "university.edu",
    "travelagency.com", "flightbooking.net", "hotelsite.org", "tourismwebsite.com", "destinationhub.net", "citytour.org",
    "fooddelivery.com", "recipewebsite.net", "restaurantsearch.org", "localservices.com", "storefinder.net", "shoppinghub.org",
    "hardwarestore.com", "electronicsnet.net", "softwaremarket.org", "gameplatform.com", "digitalplatform.net", "contentcreator.org",
    "photographywebsite.com", "designsite.net", "artportfolio.org", "businesswebsite.com", "consultingfirm.net", "financehub.org",
    "healthcare.com", "medicineshop.net", "wellnesscenter.org", "doctorappointment.com", "medportal.net", "healthnews.org"
]

# Generate random CSV data
csv_filename = "/mnt/data/google_chrome_passwords.csv"

with open(csv_filename, mode="w", newline='') as file:
    writer = csv.writer(file)
    
    # Write header
    writer.writerow(["name", "url", "username", "password"])
    
    # Write data for 60 websites
    for website in websites:
        # Generate random username based on website name
        username = f"user@{website}"
        
        # Generate a random password
        password = generate_random_password(12)
        
        # Write website info to CSV
        writer.writerow([website.split('.')[0], f"https://{website}", username, password])

csv_filename
