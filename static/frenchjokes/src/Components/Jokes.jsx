import { useEffect, useState } from 'react'
import { motion } from 'framer-motion'

//const apiKey = import.meta.env.VITE_API_KEY

const apiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoiODQ0MjU0ODU5MjU5NTQzNTYyIiwibGltaXQiOjEwMCwia2V5IjoieFJRYkxMU3JUQXFNY2FMaHFQSVVSd3ZDUmowTVo4Z014eWJRVEdRVlVFVkphQmFoS1QiLCJjcmVhdGVkX2F0IjoiMjAyNC0wNi0yNVQyMToxMzoxMSswMDowMCIsImlhdCI6MTcxOTM0OTk5MX0.vJqU2r85exI5_evFAsn-oEUQjcl1hWn3fP5btLlHaiI";

const OPTIONS = [
	{ id: 1, type: 'dev', name: 'Dev' },
	{ id: 2, type: 'limit', name: 'Adults' },
	{ id: 3, type: 'blondes', name: 'Blondes' },
	{ id: 4, type: 'global', name: 'General' },
	{ id: 5, type: 'dark', name: 'Dark ones' },
	{ id: 6, type: 'beauf', name: 'Food related' },
]

const Jokes = () => {
	const [data, setData] = useState('')
	const [type, setType] = useState('global')
	const [activeTab, setActiveTab] = useState(OPTIONS[0].id)

	useEffect(() => {
		requestJokes()
	}, [type]) // eslint-disable-line react-hooks/exhaustive-deps

	async function requestJokes() {
		fetch(`https://www.blagues-api.fr/api/type/${type}/random`, {
			headers: {
				Authorization: `Bearer ${apiKey}`,
			}, // process.env.REACT_APP_API_KEY
		})
			.then((res) => res.json())
			.then((data) => {
				setData(data)
			})
	}

	async function copyJoke() {
		const joke = data.joke + ' ' + data.answer
		try {
			await navigator.clipboard.writeText(joke)
			// console.log('Text copied to clipboard')
		} catch (error) {
			console.error('Unable to copy text: ', error)
		}
	}

	return (
		<div>
			<div className="flex justify-evenly text-center text-xl text-black mt-24">
				<div className="flex gap-x-10 justify-evenly">
					{OPTIONS.map((item) => {
						return (
							<motion.button
								whileHover={{ scale: 1.2 }}
								key={item.id}
								className={`${
									activeTab === item.id
										? 'bg-primary text-white'
										: 'bg-white text-black'
								} rounded-xl px-5 py-2`}
								onClick={() => {
									setType(item.type)
									setActiveTab(item.id)
								}}
							>
								{item.name}
							</motion.button>
						)
					})}
				</div>
			</div>

			<div className="mt-4 w-2/3 h-72 rounded-xl mx-auto bg-primary flex flex-col justify-center items-center">
				<div className="flex flex-col justify-center items-center">
					<p className="w-full px-5 text-2xl text-white text-center">
						{data.joke}
						{data.answer}
					</p>
					<div className="space-x-5 mt-20">
						<button
							onClick={(e) => {
								e.preventDefault()
								copyJoke()
							}}
							className="px-5 py-1 text-center bg-secondary rounded-xl text-white hover:bg-blue-500"
						>
							copy
						</button>
						<button className="px-5 py-1  text-center bg-secondary rounded-xl text-white hover:bg-blue-500">
							share
						</button>
						<button
							onClick={(e) => {
								e.preventDefault()

								requestJokes()
							}}
							className="px-5 py-1 bg-secondary rounded-xl text-white hover:bg-blue-500"
						>
							More
						</button>
						<span className="px-5 py-1 bg-secondary rounded-xl text-white hover:bg-blue-500">
							{data.type}
						</span>
					</div>
				</div>
			</div>
		</div>
	)
}

export default Jokes

/* 
	/api/random
	/api/type/:type:/random // type: global, dev, dark, limit, beauf, blondes
	/api/type/:type:/random // type: global, dev, dark, limit, beauf, blondes
	/api/count
	/api/random?disallow=dark
	/api/random?disallow=dark&disallow=limit&disallow=dev
*/
