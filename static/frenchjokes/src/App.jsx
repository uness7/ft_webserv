import { createRoot } from 'react-dom/client'
import { lazy, Suspense } from 'react'
import { BrowserRouter, Routes, Route } from 'react-router-dom'
import Home from './Pages/Home'

const AboutMe = lazy(() => import('./Pages/AboutMe'))

const App = () => {
	return (
		<BrowserRouter>
			<Suspense
				fallback={
					<>
						<div className="m-auto text-center text-white text-2xl">
							<h2>loading ...</h2>
						</div>
					</>
				}
			>
				<div>
					<Routes>
						<Route path="/" element={<Home />} />

						<Route path="/about-me" element={<AboutMe />} />
					</Routes>
				</div>
			</Suspense>
		</BrowserRouter>
	)
}

const container = document.getElementById('root')
const root = createRoot(container)
root.render(<App />)
