/** @type {import('tailwindcss').Config} */
module.exports = {
	content: ['./src/**/*.{js,ts,jsx,tsx,html}'],
	theme: {
		extend: {
			fontFamily: {
				roboto: ['Roboto', 'sans-serif'],
				pacifico: ['Pacifico', 'sans-serif'],
			},
			colors: {
				primary: '#FF6F00',
				secondary: '#1976D2',
				accent: '#FDA435',
			},
			spacing: {
				128: '32rem',
				256: '41rem',
			},
		},
	},
	plugins: [],
}
