const darkModeStyles = `
  .dark-mode body {
    background-color: #000000 !important;
    color: #ffffff !important;
  }
  .dark-mode .text-content-card, 
  .dark-mode .chart-card, 
  .dark-mode .information-sidebar,
  .dark-mode .settings-panel,
  .dark-mode .information-content__single,
  .dark-mode .information-links__single {
    background-color: rgba(20, 20, 20, 0.95) !important;
    color: #ffffff !important;
    border-color: rgba(255, 255, 255, 0.1) !important;
  }
  .dark-mode .information-sidebar__logo img,
  .dark-mode .user-avatar-btn img {
    filter: invert(1);
  }
  .dark-mode .user-dropdown {
    background: rgba(30, 30, 30, 0.98) !important;
    border-color: rgba(255, 255, 255, 0.1) !important;
    color: #fff !important;
  }
  .dark-mode .user-name { color: #fff !important; }
  .dark-mode .user-dropdown-list li a { color: #ccc !important; }
  .dark-mode .user-dropdown-list li a:hover { background: rgba(255, 255, 255, 0.05) !important; }
  .dark-mode .bg-word {
    background: linear-gradient(165deg, #333, #111) !important;
    -webkit-background-clip: text !important;
    background-clip: text !important;
    text-shadow: none !important;
    opacity: 0.3 !important;
  }
`;

function injectDarkMode() {
  if (!document.getElementById('dark-mode-styles')) {
    const style = document.createElement('style');
    style.id = 'dark-mode-styles';
    style.textContent = darkModeStyles;
    document.head.appendChild(style);
  }
  if (localStorage.getItem('darkMode') === 'true') {
    document.documentElement.classList.add('dark-mode');
  }
}

function toggleDarkMode() {
  const isDark = document.documentElement.classList.toggle('dark-mode');
  localStorage.setItem('darkMode', isDark);
  const toggleText = document.getElementById('theme-toggle-text');
  if (toggleText) toggleText.textContent = isDark ? 'Light Mode' : 'Dark Mode';
}

(function () {
  const documentationPage = document.querySelector(".information-page");

  if (documentationPage) {
    const documentationSidebar = documentationPage.querySelector(
      ".information-sidebar"
    );
    const documentationSidebarToggler = documentationPage.querySelector(
      ".information-sidebar__toggle"
    );

    if (documentationSidebarToggler) {
      documentationSidebarToggler.addEventListener("click", (e) => {
        e.preventDefault();
        documentationSidebar.classList.toggle("is-open");
      });
    }
  }
})();

function getSidebarContent() {
  return `
    <ul>
      <li class="menu-item">
        <a href="/inicio">Home</a>
      </li>
      <li class="menu-item menu-item-has-children">
        <a href="#">Features</a>
        <ul>
          <li class="menu-item">
            <a href="/lista-boyas">Buoys</a>
          </li>
          <li class="menu-item">
            <a href="/lista-torretas">Turrets</a>
          </li>
        </ul>
      </li>
      <li class="menu-item menu-item-has-children">
        <a href="#">Information</a>
        <ul>
          <li class="menu-item">
            <a href="/sistema">System</a>
          </li>
        </ul>
      </li>
      <li class="menu-item">
        <a href="./configuracion">Settings</a>
      </li>
    </ul>
  `;
}

function renderSidebar() {
  const sidebarContainer = document.getElementById("sidebar-menu-content");
  if (sidebarContainer) {
    sidebarContainer.innerHTML = getSidebarContent();
  }
}

function highlightCurrentMenuItem() {
  const currentPath = decodeURIComponent(window.location.pathname);
  const menuItems = document.querySelectorAll(
    ".information-sidebar .menu-item a"
  );

  menuItems.forEach((item) => {
    let itemPath = item.getAttribute("href").replace(/^\.\//, "");
    if (currentPath.includes(itemPath)) {
      item.parentElement.classList.add("current-menu-item");
    } else {
      item.parentElement.classList.remove("current-menu-item");
    }
  });
}

function getUserMenuContent() {
  const isDark = localStorage.getItem('darkMode') === 'true';
  return `
    <div class="user-menu-wrapper">
      <button class="user-avatar-btn" id="user-avatar-btn">
        <img src="/images/users.svg" alt="User" />
      </button>
      <div class="user-dropdown" id="user-dropdown">
        <div class="user-dropdown-header">
          <span class="user-name">user</span>
          <span class="user-role">Administrator</span>
        </div>
        <div class="user-dropdown-divider"></div>
        <ul class="user-dropdown-list">
          <li><a href="#"><img src="/images/users.svg" style="height:14px; margin-right:8px; opacity:0.6;"> Account</a></li>
          <li><a href="#"><img src="/images/VitalNet-Logo.svg" style="height:14px; margin-right:8px; opacity:0.6;"> About Us</a></li>
          <li><a href="./configuracion"><img src="/images/config.svg" style="height:14px; margin-right:8px; opacity:0.6;"> Settings</a></li>
          <li><a href="javascript:void(0)" id="dark-mode-toggle">
            <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="margin-right:8px; opacity:0.6;">
              <path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"></path>
            </svg>
            <span id="theme-toggle-text">${isDark ? 'Light Mode' : 'Dark Mode'}</span>
          </a></li>
          <li class="logout"><a href="/login.html"><svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="#ff3b30" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="margin-right:8px; opacity:0.8;"><path d="M9 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h4"></path><polyline points="16 17 21 12 16 7"></polyline><line x1="21" y1="12" x2="9" y2="12"></line></svg> Log out</a></li>
        </ul>
      </div>
    </div>
  `;
}

function renderUserMenu() {
  let menuContainer = document.getElementById("user-menu-container");
  if (!menuContainer) {
    menuContainer = document.createElement('div');
    menuContainer.id = "user-menu-container";
    document.body.appendChild(menuContainer);
  }
  menuContainer.innerHTML = getUserMenuContent();

  const avatarBtn = document.getElementById("user-avatar-btn");
  const dropdown = document.getElementById("user-dropdown");
  const darkToggle = document.getElementById("dark-mode-toggle");

  if (avatarBtn && dropdown) {
    avatarBtn.onclick = function (e) {
      e.stopPropagation();
      dropdown.classList.toggle("is-active");
    };

    document.addEventListener("click", function () {
      dropdown.classList.remove("is-active");
    });

    dropdown.onclick = function (e) { e.stopPropagation(); };
  }

  if (darkToggle) {
    darkToggle.onclick = function (e) {
      e.preventDefault();
      toggleDarkMode();
    };
  }
}

document.addEventListener("DOMContentLoaded", function () {
  injectDarkMode();
  renderSidebar();
  renderUserMenu();
  highlightCurrentMenuItem();
});