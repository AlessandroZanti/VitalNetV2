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
  .dark-mode .information-links__single,
  .dark-mode main {
    background-color: rgba(18, 18, 18, 0.95) !important;
    color: #ffffff !important;
    border: 1px solid rgba(255, 255, 255, 0.1) !important;
  }
  .dark-mode input[type="text"],
  .dark-mode input[type="password"],
  .dark-mode input[type="email"],
  .dark-mode input[type="tel"],
  .dark-mode select,
  .dark-mode textarea {
    background-color: rgba(255, 255, 255, 0.05) !important;
    color: #ffffff !important;
    border-color: rgba(255, 255, 255, 0.2) !important;
  }
  .dark-mode ::placeholder {
    color: rgba(255, 255, 255, 0.5) !important;
  }
  .dark-mode option {
    background-color: #333;
    color: #fff;
  }
  .dark-mode label, .dark-mode h1, .dark-mode .create-account {
    color: #ffffff !important;
  }
  .dark-mode .information-sidebar__logo img,
  .dark-mode .user-avatar-btn img,
  .dark-mode .user-dropdown-list li img,
  .dark-mode .information-links__icon img,
  .dark-mode .site-header img,
  .dark-mode .footer-elements img,
  .dark-mode main img {
    filter: invert(1) brightness(2);
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
  .dark-mode #theme-toggle-btn {
    background: rgba(30, 30, 30, 0.9) !important;
    border-color: rgba(255, 255, 255, 0.1) !important;
    color: #fff !important;
  }
`;

const sunIconContent = `<circle cx="12" cy="12" r="5"></circle><line x1="12" y1="1" x2="12" y2="3"></line><line x1="12" y1="21" x2="12" y2="23"></line><line x1="4.22" y1="4.22" x2="5.64" y2="5.64"></line><line x1="18.36" y1="18.36" x2="19.78" y2="19.78"></line><line x1="1" y1="12" x2="3" y2="12"></line><line x1="21" y1="12" x2="23" y2="12"></line><line x1="4.22" y1="19.78" x2="5.64" y2="18.36"></line><line x1="18.36" y1="5.64" x2="19.78" y2="4.22"></line>`;
const moonIconContent = `<path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"></path>`;

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

function updateThemeIcons() {
  const isDark = document.documentElement.classList.contains('dark-mode');
  const iconSize = "14";

  // Helper to create SVG
  const createSVG = (content, isMenu = false) => {
    const style = isMenu ? 'margin-right:8px; opacity:0.6;' : '';
    return `<svg width="${iconSize}" height="${iconSize}" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="${style}">${content}</svg>`;
  };

  // Update toggle button in user menu (if it exists)
  const menuIconContainer = document.querySelector('#dark-mode-toggle svg');
  if (menuIconContainer) {
    menuIconContainer.outerHTML = createSVG(isDark ? moonIconContent : sunIconContent, true);
  }
  const menuText = document.getElementById('theme-toggle-text');
  if (menuText) {
    menuText.textContent = isDark ? 'Light Mode' : 'Dark Mode';
  }

  // Update floating button (login/register pages)
  const floatingBtn = document.getElementById('theme-toggle-btn');
  if (floatingBtn) {
    floatingBtn.innerHTML = createSVG(isDark ? moonIconContent : sunIconContent, false);
    if (!floatingBtn.onclick) {
      floatingBtn.onclick = function () { toggleDarkMode(); };
    }
  }
}

function toggleDarkMode() {
  const isDark = document.documentElement.classList.toggle('dark-mode');
  localStorage.setItem('darkMode', isDark);
  updateThemeIcons();
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

function isAuthPage() {
  const path = window.location.pathname.toLowerCase();
  return path.includes('login') ||
    path.includes('register') ||
    path === '/' ||
    path === '' ||
    document.getElementById('loginForm') !== null ||
    document.getElementById('registerForm') !== null;
}

function renderSidebar() {
  if (isAuthPage()) return;
  const sidebarContainer = document.getElementById("sidebar-menu-content");
  if (sidebarContainer) {
    sidebarContainer.innerHTML = getSidebarContent();
  }
}

function highlightCurrentMenuItem() {
  if (isAuthPage()) return;
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
  if (isAuthPage()) return;
  let menuContainer = document.getElementById("user-menu-container");

  if (!menuContainer) {
    menuContainer = document.createElement('div');
    menuContainer.id = "user-menu-container";
    document.body.appendChild(menuContainer);
  }

  if (menuContainer) {
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
}

document.addEventListener("DOMContentLoaded", function () {
  injectDarkMode();
  renderSidebar();
  renderUserMenu();
  updateThemeIcons();
  highlightCurrentMenuItem();
});